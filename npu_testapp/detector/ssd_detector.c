#include "./detector.h"

#ifdef DUMP
#include <stdio.h>
#endif

objs_t objects;


static void build_box_info(box_t* box, tensor_t* loc, tensor_t* prior, int idx, int stride)
{
    int loc_idx = idx * stride;
    int box_idx = idx << 2;

    int8_t* _loc = (int8_t*)loc->buf + loc_idx;
    uint8_t* _prior = (uint8_t*)prior->buf + box_idx;

    uint32_t* table = (uint32_t*)loc->exp_tbl;

    int x, y, w, h, x_center, y_center, x_offset, y_offset;

    const int S_cal = 8;
    const int S_tbl = 8;
    int l2s;

    // w, h scale
    // S_w = S_prior * S_tbl
    // S_h = S_prior * S_tbl
    w = (int)_prior[2] * table[_loc[2]];
    h = (int)_prior[3] * table[_loc[3]];

    l2s = (prior->scl + S_tbl) - S_cal;
    if (l2s > 0) {
        w >>= l2s;
        h >>= l2s;
    }
    else {
        w <<= -l2s;
        h <<= -l2s;
    }

    // x_offset, y_offset scale
    // S_xoff = S_loc * S_prior
    x_offset = (int)_loc[0] * (int)_prior[2];
    y_offset = (int)_loc[1] * (int)_prior[3];

    l2s = (loc->scl + prior->scl) - S_cal;
    if (l2s > 0) {
        x_offset >>= l2s;
        y_offset >>= l2s;
    }
    else {
        x_offset <<= -l2s;
        y_offset <<= -l2s;
    }

    l2s = prior->scl - S_cal;
    if (l2s > 0) {
        x_center = _prior[0] >> l2s;
        y_center = _prior[1] >> l2s;
    }
    else {
        x_center = _prior[0] << (-l2s);
        y_center = _prior[1] << (-l2s);
    }

    x = x_center - w / 2 + x_offset;
    y = y_center - h / 2 + y_offset;

    box->x_min = x;
    box->x_max = x + w;
    box->y_min = y;
    box->y_max = y + h;
}

// input: { batch, boxes, classes }
// output: { boxes { coordinate, score } }
static void thresholding(tensor_t* scr, cand_t* cand,
                         tensor_t* loc, tensor_t* prior,
                         int class, uint8_t th, int stride)
{
    uint8_t* _scr = scr->buf + class;

    int num_box = scr->dims[1];
    int num_class = scr->dims[2];

    int box, n;

    for (box = 0, n = 0 ; box < num_box ; box++, _scr += num_class) {
        if (*_scr >= th) {
            build_box_info(&cand->box[n], loc, prior, box, stride);
            cand->class[n] = class;
            cand->score[n] = *_scr;
            n++;
        }
    }
    cand->n = n;
}

static void thresholding_per_default_box(tensor_t* scr, cand_t* cand,
                                         tensor_t* loc, tensor_t* prior,
                                         uint8_t th, int scr_stride, int loc_stride, int has_bg)
{
    uint8_t* _scr = scr->buf + has_bg;

    int num_box = scr->dims[1];
    int num_class = scr->dims[2];

    int box, n, cls;

    for (box = 0, n = 0 ; box < num_box; box++, _scr += scr_stride) {
        int max_idx = 0;
        int max_val = _scr[0];
        for (cls = 1; cls < num_class - has_bg; cls++){
            if (max_val < _scr[cls]) {
                max_val = _scr[cls];
                max_idx = cls;
            }
        }

        if (max_val >= th) {
            build_box_info(&cand->box[n], loc, prior, box, loc_stride);
            cand->class[n] = max_idx;
            cand->score[n] = max_val;
            n++;
        }
    }
    cand->n = n;
}


static void thresholding_with_candidate(tensor_t* score, cand_t* cand,
                                        tensor_t* loc, tensor_t* prior,
                                        int *cand_box, int num_cand_box, 
                                        int class, uint8_t th, int stride)
{
    uint8_t* _score = score->buf + class;
    uint8_t* current_score;

    int num_class = score->dims[2];

    int i, box_idx;
    int n = 0;

    for (i = 0; i < num_cand_box; i++) {
        box_idx = cand_box[i];
        current_score = num_class * i + _score;
        if (*current_score >= th) {
            build_box_info(&cand->box[n], loc, prior, box_idx, stride);
            cand->class[n] = class;
            cand->score[n] = *current_score;
            n++;
        }
    }
    cand->n = n;
}

#define MAX_NMS_CAND 256

detection_t* ssd_post_process(tensor_t* score, tensor_t* loc, tensor_t* pri,
                              uint8_t th_conf, uint8_t th_nms, const int word_size,
                              int bg_in_cls, int logistic, int nms_method)
{
    detection_t* detection = alloc_detection(score->num_ele);

    // batches, boxes, classes
    int len_loc = loc->dims[2];

    int num_box = score->dims[1];
    int num_class = score->dims[2];

    int conf_stride = (num_class + (word_size - 1)) & ~ (word_size - 1);
    int loc_stride = (len_loc + (word_size - 1)) & ~ (word_size - 1);

    int class;

    int num_cand_box;
    int *cand_box = (int*)enlight_malloc(4 * MAX_NMS_CAND);
  
    tensor_t* conf;
    int dst_dims[3];
    int i;

    for (i = 0; i < score->num_dim; i++)
        dst_dims[i] = score->dims[i];
    dst_dims[1] = MAX_NMS_CAND;

    if (logistic == 0) {
        conf = score;
    }
    else if (logistic == 1){
        conf = alloc_tensor(TYPE_UINT8, score->num_dim, dst_dims, 0);
        //sigmoid(score, conf, conf_stride);
        sigmoid_with_threshold(score, conf, cand_box, &num_cand_box,
                               conf_stride, th_conf, bg_in_cls, MAX_NMS_CAND);
    }
    else {
        conf = alloc_tensor(TYPE_UINT8, score->num_dim, dst_dims, 0);
        softmax_with_threshold(score, conf, cand_box, &num_cand_box,
                               conf_stride, th_conf, bg_in_cls, MAX_NMS_CAND);
    }

#ifdef DUMP
    if (num_cand_box > 0) {
        FILE *fp;
        fp = fopen("regression_result.bin", "wb");
        fwrite(conf->buf, 1, num_cand_box * num_class, fp);
        fclose(fp);
    }
#endif

    if (nms_method == 0) {

        for (class = bg_in_cls; class < num_class; class++)
        {
            int num_box = conf->dims[1];
            cand_t* cand = (cand_t*)alloc_candidate(num_box);

            thresholding_with_candidate(conf, cand, loc, pri, cand_box,
                                        num_cand_box, class, th_conf, loc_stride);

#ifdef DUMP
            int num_cand = cand->n;
            int i;
            char dump_name[256];
            FILE *fp;

            sprintf(dump_name, "cand_decoded_box_%d.bin", class);
            if(num_cand > 0) {
                fp = fopen(dump_name, "wb");
                int16_t *box = (int16_t*)malloc(sizeof(int16_t) * num_cand * 4);
                for(i = 0 ; i < num_cand; i++) {
                    box[4 * i + 0] = cand->box[i].x_min;
                    box[4 * i + 1] = cand->box[i].y_min;
                    box[4 * i + 2] = cand->box[i].x_max;
                    box[4 * i + 3] = cand->box[i].y_max;
                }
                fwrite(box, 2, 4 * num_cand, fp);
                fclose(fp);
                free(box);
            }
#endif
            nms(detection, cand, class, th_nms);
            free_candidate(cand);
        }
    }
    else {
        cand_t* cand = (cand_t*)alloc_candidate(num_box);

        thresholding_per_default_box(conf, cand, loc, pri, th_conf,
                                     conf_stride, loc_stride, bg_in_cls);

#ifdef DUMP
        int num_cand = cand->n;

        if (num_cand > 0) {
            int i;
            char dump_name[256];
            FILE *fp;

            sprintf(dump_name, "cand_decoded_box.bin");
            fp = fopen(dump_name, "wb");
            int16_t *box = (int16_t*)malloc(sizeof(int16_t) * num_cand * 4);
            for(i = 0 ; i < num_cand; i++) {
                box[4 * i + 0] = cand->box[i].x_min;
                box[4 * i + 1] = cand->box[i].y_min;
                box[4 * i + 2] = cand->box[i].x_max;
                box[4 * i + 3] = cand->box[i].y_max;
            }
            fwrite(box, 2, 4 * num_cand, fp);
            fclose(fp);
            free(box);

            sprintf(dump_name, "cand_score.bin");
            fp = fopen(dump_name, "wb");
            int8_t *cand_sc = (int8_t*)malloc(sizeof(int8_t) * num_cand);
            for (i = 0; i < num_cand; i++)
                cand_sc[i] = cand->score[i];
            fwrite(cand_sc, 1, num_cand, fp);
            fclose(fp);
            free(cand_sc);

            sprintf(dump_name, "cand_class.bin");
            fp = fopen(dump_name, "wb");
            uint8_t *cand_cls = (uint8_t*)malloc(sizeof(uint8_t) * num_cand);
            for (i = 0; i < num_cand; i++)
                cand_cls[i] = cand->class[i];
            fwrite(cand_cls, 1, num_cand, fp);
            fclose(fp);
            free(cand_cls);
        }
#endif
        fastnms(detection, cand, th_nms);
        free_candidate(cand);
    }

    enlight_free((uint8_t*)cand_box);

    if (logistic != 0)
        free_tensor(conf);

    return detection;
}

static int ssd_detector(void* pri_buf, void* loc_buf, void* score_buf,
                        uint32_t *loc_exp_tbl, uint32_t *score_logistic_tbl,
                        int pri_scl, int loc_scl, int score_scl,
                        int num_class, int num_box, int img_h, int img_w,
                        int th_conf, int th_iou,
                        int bg_in_cls, int logistic, int nms_method)
{

    const int word_size = 32;
    int i;
    int log2_scr_result_scl = 8;
    int log2_box_result_scl = 8;

    volatile unsigned long cycle0, cycle1, cycle0_h, cycle1_h;

    tensor_t scr, loc, pri;

    pri.buf     = pri_buf;
    pri.scl     = pri_scl;
    pri.dtype   = TYPE_UINT8;
    pri.num_ele = num_box*4;
    pri.num_dim = 2;
    pri.dims[0] = num_box;
    pri.dims[1] = 4;
    pri.exp_tbl = NULL;

    loc.buf     = loc_buf;
    loc.scl     = loc_scl;
    loc.dtype   = TYPE_INT8;
    loc.num_ele = 1*num_box*4;
    loc.num_dim = 3;
    loc.dims[0] = 1;
    loc.dims[1] = num_box;
    loc.dims[2] = 4;
    loc.exp_tbl = &loc_exp_tbl[128];

    scr.buf     = score_buf;
    scr.scl     = score_scl;
    scr.dtype   = TYPE_INT8;
    scr.num_ele = 1*num_box*num_class;
    scr.num_dim = 3;
    scr.dims[0] = 1;
    scr.dims[1] = num_box;
    scr.dims[2] = num_class;

    if (logistic == 0)
        log2_scr_result_scl = 6;
    else if (logistic == 1)
        scr.sig_tbl = &score_logistic_tbl[128];
    else
        scr.exp_tbl = &score_logistic_tbl[128];


#ifdef DETECTOR_DEBUG
    enlight_log(" pri.scl          %08x\n",     pri.scl);
    enlight_log(" pri.dtype        %08x\n",     pri.dtype);
    enlight_log(" pri.num_ele      %08x\n",     pri.num_ele);
    enlight_log(" pri.num_dim      %08x\n",     pri.num_dim);
    enlight_log(" pri.dims[0]      %08x\n",     pri.dims[0]);
    enlight_log(" pri.dims[1]      %08x\n",     pri.dims[1]);
    enlight_log(" pri.table        %08x\n",     pri.exp_tbl);
    enlight_log("                  %08x\n",     0);
    enlight_log(" loc.buf          %08x\n",     loc.buf );
    enlight_log(" loc.scl          %08x\n",     loc.scl);
    enlight_log(" loc.dtype        %08x\n",     loc.dtype);
    enlight_log(" loc.num_ele      %08x\n",     loc.num_ele);
    enlight_log(" loc.num_dim      %08x\n",     loc.num_dim);
    enlight_log(" loc.dims[0]      %08x\n",     loc.dims[0]);
    enlight_log(" loc.dims[1]      %08x\n",     loc.dims[1]);
    enlight_log(" loc.dims[2]      %08x\n",     loc.dims[2]);
    enlight_log(" loc.table        %08x\n",     loc.exp_tbl);
    enlight_log("                  %08x\n",     0);
    enlight_log(" scr.buf          %08x\n",     scr.buf);
    enlight_log(" scr.scl          %08x\n",     scr.scl);
    enlight_log(" scr.dtype        %08x\n",     scr.dtype);
    enlight_log(" scr.num_ele      %08x\n",     scr.num_ele);
    enlight_log(" scr.num_dim      %08x\n",     scr.num_dim);
    enlight_log(" scr.dims[0]      %08x\n",     scr.dims[0]);
    enlight_log(" scr.dims[1]      %08x\n",     scr.dims[1]);
    enlight_log(" scr.dims[2]      %08x\n",     scr.dims[2]);
    enlight_log(" scr.table        %08x\n",     scr.exp_tbl);
#endif

    enlight_log("SSD Postproc start\n");

#ifdef __i386__
    asm volatile ("rdtsc" : "=a"(cycle0), "=d"(cycle0_h));
#else
    asm volatile ("rdcycle %0" : "=r" (cycle0));
#endif

    detection_t* r1 = ssd_post_process(&scr, &loc, &pri, th_conf, th_iou, word_size,
                                       bg_in_cls, logistic, nms_method);

#ifdef __i386__
    asm volatile ("rdtsc" : "=a"(cycle1), "=d"(cycle1_h));
#else
    asm volatile ("rdcycle %0" : "=r" (cycle1));
#endif
    enlight_log("[Postproc cycles] %ld \n", cycle1 - cycle0);

    enlight_log("%d object is detected\n", r1->n);

#ifdef DUMP
    if(r1->n > 0) {
        uint16_t *box_write = (uint16_t*)malloc(2 * 4 * r1->n);
        for(i = 0 ; i < r1->n; i++) {
            box_write[4 * i + 0] = r1->box[i].x_min;
            box_write[4 * i + 1] = r1->box[i].y_min;
            box_write[4 * i + 2] = r1->box[i].x_max;
            box_write[4 * i + 3] = r1->box[i].y_max;
        }
        FILE *fp = fopen("nms_box_info.bin", "wb");
        fwrite(box_write, 2, 4 * r1->n, fp);
        fclose(fp);
        free(box_write);
    }
#endif

    for (i = 0 ; i < r1->n ; i++)
    {
        box_t* b = r1->box + i;

        enlight_log("x_min: %d, x_max: %d, y_min: %d, y_max: %d, score: %d, class: %d\n", 
                img_w*b->x_min/(1 << log2_box_result_scl),
                img_w*b->x_max/(1 << log2_box_result_scl),
                img_h*b->y_min/(1 << log2_box_result_scl),
                img_h*b->y_max/(1 << log2_box_result_scl),
                ((int)r1->score[i])*100/(1 << log2_scr_result_scl),
                r1->class[i]);

        obj_t *detect_obj = &objects.obj[objects.cnt];
        detect_obj->x_min = img_w*b->x_min/(1 << log2_box_result_scl);
        detect_obj->x_max = img_w*b->x_max/(1 << log2_box_result_scl);
        detect_obj->y_min = img_h*b->y_min/(1 << log2_box_result_scl);
        detect_obj->y_max = img_h*b->y_max/(1 << log2_box_result_scl);
        detect_obj->score = (r1->score[i] * 100)/(1 << log2_scr_result_scl);
        detect_obj->class = r1->class[i];
        detect_obj->img_w = img_w;
        detect_obj->img_h = img_h;
        objects.cnt++;
    }

    enlight_free((uint8_t*)r1);
    return 0;
}

void ssd_detector_init(void *work_buf)
{ 
    objects.cnt=0;
#ifndef SUPPORT_MALLOC
    enlight_init_mem(work_buf);
#endif
}

void ssd_detector_run(
    uint8_t *prior, uint8_t *loc, uint8_t *score,
    uint32_t *loc_exp_tbl, uint32_t *score_logistic_tbl,
    int prior_scl, int loc_scl, int score_scl,
    int num_class, int num_box, int img_h, int img_w,
    int th_conf, int th_iou,  
    int bg_in_cls, int logistic, int nms_method)
{
    ssd_detector(prior, loc, score,
                 loc_exp_tbl, score_logistic_tbl,
                 prior_scl, loc_scl, score_scl,
                 num_class, num_box, img_h, img_w,
                 (uint8_t)th_conf, (uint8_t)th_iou,
                 bg_in_cls, logistic, nms_method);
}

