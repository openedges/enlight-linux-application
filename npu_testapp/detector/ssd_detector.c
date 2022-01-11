#include "./detector.h"

static objs_t objects;

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
static void thresholding(tensor_t* score, cand_t* cand,
                          tensor_t* loc, tensor_t* prior,
                          int class, uint8_t th, int stride)
{
    uint8_t* _score = score->buf + class;

    int num_box = score->dims[1];
    int num_class = score->dims[2];

    int box, n;
    int prior_offset;

    for (box = 0, n = 0 ; box < num_box ; box++, _score += num_class) {
        if (*_score >= th) {
            build_box_info(&cand->box[n], loc, prior, box, stride);
            cand->class[n] = class;
            cand->score[n] = *_score;
            n++;
        }
    }
    cand->n = n;
}


detection_t* ssd_post_process(tensor_t* score, tensor_t* loc, tensor_t* pri,
                          uint8_t th_conf, uint8_t th_nms, int stride)
{
    detection_t* detection = alloc_detection(score->num_ele);

    // batches, boxes, classes
    int num_class = score->dims[2];
    int class;

    cand_t* candidate;
    tensor_t* conf;

    conf = alloc_tensor(TYPE_UINT8, score->num_dim, score->dims, 0);

    softmax(score, conf, stride);

    for (class = 1 ; class < num_class ; class++)
    {
        int num_box = conf->dims[1];
        cand_t* cand = (cand_t*)alloc_candidate(num_box);

        thresholding(conf, cand, loc, pri, class, th_conf, stride);

        nms(detection, cand, class, th_nms);

        free_candidate(cand);
    }

    free_tensor(conf);

    return detection;
}

static int ssd_detector(void* pri_buf, void* loc_buf, void* score_buf,
            uint32_t *loc_exp_tbl, uint32_t *score_exp_tbl,
            int pri_scl, int loc_scl, int score_scl,
            int num_class, int num_box, int img_size,
            int th_conf, int th_iou)
{

    const int tensor_buf_stride = 32;
    int i;
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
    scr.exp_tbl = &score_exp_tbl[128];

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

    detection_t* r1 = ssd_post_process(&scr, &loc, &pri, th_conf, th_iou, tensor_buf_stride);

#ifdef __i386__
    asm volatile ("rdtsc" : "=a"(cycle1), "=d"(cycle1_h));
#else
    asm volatile ("rdcycle %0" : "=r" (cycle1));
#endif
    enlight_log("[Postproc cycles] %ld \n", cycle1 - cycle0);

    enlight_log("%d object is detected\n", r1->n);
    
    for (i = 0 ; i < r1->n ; i++)
    {
        box_t* b = r1->box + i;

        enlight_log("x_min: %d, x_max: %d, y_min: %d, y_max: %d, score: %d, class: %d\n", 
                img_size*b->x_min/256,
                img_size*b->x_max/256,
                img_size*b->y_min/256,
                img_size*b->y_max/256,
                ((int)r1->score[i])*100/256,
                r1->class[i]);

        obj_t *detect_obj = &objects.obj[objects.cnt];
        detect_obj->x_min = img_size*b->x_min/256;
        detect_obj->x_max = img_size*b->x_max/256;
        detect_obj->y_min = img_size*b->y_min/256;
        detect_obj->y_max = img_size*b->y_max/256;
        detect_obj->class = r1->score[i];
        detect_obj->score = r1->class[i];
        detect_obj->img_w = img_size;
        detect_obj->img_h = img_size;
        objects.cnt++;
    }

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
    uint8_t *prior_box, uint8_t *loc_buf_32, uint8_t *score_buf_32,
    uint32_t *loc_exp_tbl, uint32_t *score_exp_tbl,
    int prior_scl, int loc_scl, int score_scl,
    int stages, int *grids,
    int num_class, int num_box, int img_size,
    int th_conf, int th_iou) 
{
    //detector(prior_box, loc_buf_32_dst, score_buf_32,
    ssd_detector(prior_box, loc_buf_32, score_buf_32,
             loc_exp_tbl, score_exp_tbl,
             prior_scl, loc_scl, score_scl,
             num_class, num_box, img_size,
             (uint8_t)th_conf, (uint8_t)th_iou);
}

