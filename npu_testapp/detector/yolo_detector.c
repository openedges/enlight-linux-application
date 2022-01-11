#include "detector.h"

#ifdef DUMP
#include <stdio.h>
#endif

extern objs_t objects;

static void build_box_info(box_t* box, tensor_t* loc, tensor_t* prior, int idx, int* num_grid)
{
    int loc_idx = idx << 2;
    int box_idx = idx << 2;

    int8_t* _loc = (int8_t*)loc->buf + loc_idx;
    uint8_t* _prior = (uint8_t*)prior->buf + box_idx;

    uint8_t* sig_tbl = (uint8_t*)loc->sig_tbl;
    uint32_t* exp_tbl = (uint32_t*)loc->exp_tbl;

    int x, y, w, h, anchor_x, anchor_y;

    x = (int)sig_tbl[_loc[0]];
    y = (int)sig_tbl[_loc[1]];

    // xy / num_grid
    uint32_t compensation_bit = 16;
    uint32_t compensation_x = (1 << compensation_bit) / num_grid[0];
    uint32_t compensation_y = (1 << compensation_bit) / num_grid[1];
    uint32_t temp;

    temp = (uint32_t)x * compensation_x;
    x = (int)(temp >> compensation_bit);
    temp = (uint32_t)y * compensation_y;
    y = (int)(temp >> compensation_bit);

    const int log2_result_scl = 8;
    const int log2_sigmoid_scl = 8;
    const int log2_exp_scl = 8;

    int log2_cal_scl;

    log2_cal_scl = log2_result_scl - log2_sigmoid_scl;

    if(log2_cal_scl > 0) {
        x <<= log2_cal_scl;
        y <<= log2_cal_scl;
    }
    else {
        x >>= -log2_cal_scl;
        y >>= -log2_cal_scl;
    }

    log2_cal_scl = log2_result_scl - prior->scl;

    anchor_x = (int)_prior[0];
    anchor_y = (int)_prior[1];

    if(log2_cal_scl > 0) {
        anchor_x <<= log2_cal_scl;
        anchor_y <<= log2_cal_scl;
    }
    else {
        anchor_x >>= -log2_cal_scl;
        anchor_y >>= -log2_cal_scl;
    }

    x = x + anchor_x;
    y = y + anchor_y;

    log2_cal_scl = log2_result_scl - (log2_exp_scl + prior->scl);

    w = (int)_prior[2] * exp_tbl[_loc[2]];
    h = (int)_prior[3] * exp_tbl[_loc[3]];

    if(log2_cal_scl > 0) {
        w <<= log2_cal_scl;
        h <<= log2_cal_scl;
    }
    else {
        w >>= -log2_cal_scl;
        h >>= -log2_cal_scl;
    }

    w >>= 1;
    h >>= 1;

    box->x_min = x - w;
    box->x_max = x + w;
    box->y_min = y - h;
    box->y_max = y + h;
}


// input: { batch, boxes, classes }
// output: { boxes { coordinate, score } }
static void thresholding(tensor_t** score, cand_t* cand,
                         tensor_t* loc, tensor_t* prior,
                         int class, uint8_t th, int* num_grids, int num_output)
{
    uint8_t* _score;
    int num_box;
    int num_class = score[0]->dims[2];

    int i, box;
    int n = 0;
    for(i = 0 ; i < num_output; i++) {
        _score = score[i]->buf + class;
        num_box = score[i]->dims[1];

        for (box = 0; box < num_box ; box++, _score += num_class) {
            if (*_score >= th) {
                build_box_info(&cand->box[n], &loc[i], &prior[i], box, &num_grids[i*2]);
                cand->class[n] = class;
                cand->score[n] = *_score;
                n++;
            }
        }
    }
    cand->n = n;
}


detection_t* yolo_post_process(
    tensor_t* score, tensor_t* loc, tensor_t* pri, tensor_t* obj,
    uint8_t th_conf, uint8_t th_nms, int* num_grids, int num_output, int softmax_use)
{
    detection_t* detection;
    int i, j, c;
    int num_cls, num_box;
    int total_num_box = 0;
    uint8_t *conf_buf, *obj_buf;
    tensor_t* conf[3];

    int num_ele = 0;
    for(i = 0; i < num_output; i++)
        num_ele += score[i].num_ele;
    
    detection = alloc_detection(num_ele);
    num_cls = score[0].dims[2];

    for(i =0; i < num_output; i++) {
        num_box = score[i].dims[1];
        total_num_box += num_box;

        conf[i] = alloc_tensor(TYPE_UINT8, score[i].num_dim, score[i].dims, 0);

        if (softmax_use)
            softmax(&score[i], conf[i], score[i].dims[2]);
        else
            sigmoid(&score[i], conf[i], score[i].dims[2]);

        conf_buf = conf[i]->buf;
        obj_buf = obj[i].buf;

#ifdef DUMP
        char dump_name[256];
        FILE *fp;
        if (num_box > 0) {
            sprintf(dump_name, "softmax_or_sigmoid_%d.bin", i);
            fp = fopen(dump_name, "wb");
            fwrite(conf_buf, 1, num_box * num_cls, fp);
            fclose(fp);
        }
#endif
        for(j = 0; j < num_box; j++) {
            for(c = 0; c < num_cls; c++) {
                *conf_buf = ((*conf_buf) * (*obj_buf)) >> 8;
                conf_buf++;
            }
            obj_buf++;
        }
#ifdef DUMP
        if (num_box > 0) {
            sprintf(dump_name, "mul_conf_with_obj_%d.bin", i);
            fopen(dump_name, "wb");
            fwrite(conf[i]->buf, 1, num_box * num_cls, fp);
            fclose(fp);
        }
#endif
    }

    for (c = 0; c < num_cls; c++) {
        cand_t* cand = (cand_t*)alloc_candidate(total_num_box);

        thresholding(conf, cand, loc, pri, c, th_conf, num_grids, num_output);
        nms(detection, cand, c, th_nms);

        free_candidate(cand);
    }

    for(i = 0; i < num_output; i++)
        free_tensor(conf[i]);

    return detection;
}

//#define DEBUG_GET_CAND
static void get_candidate(void** output_addr, int** cand_buf_addr, int *cand_nums, uint8_t** sig_tbl_addr,
                         int num_output, int* num_grids, int num_anchor, int num_class,
                         int th_conf)
{
    const int word = 32;
    const int coord_len = 4;
    const int obj_len = 1;

    int cand_cnt;

    int box_len = coord_len + obj_len + num_class;
    int ch = num_anchor * box_len;
    int aligned_ch = (ch + (word - 1)) & ~ (word - 1);

    int i, j, k;

#ifdef DEBUG_GET_CAND
    uint8_t *data;
    for(i = 0 ; i < num_output; i++) {

        data = (uint8_t*)output_addr[i];
        for (j = 0; j < num_grids[i*2 + 0] * num_grids[i*2 + 1]; j++) {

            for (k = 0; k < num_anchor; k++) {
                enlight_log("[%3d]%3x %3x %3x %3x  %3d  %3d %3d | ",
                    j*num_anchor + k,
                    data[k*25 + 0],
                    data[k*25 + 1],
                    data[k*25 + 2],
                    data[k*25 + 3],
                    (int8_t)data[k*25 + 4],
                    (int8_t)data[k*25 + 5],
                    (int8_t)data[k*25 + 6]);
            }
            enlight_log("\n");
            data += aligned_ch;
        }
    }
#endif
    int8_t *src, *output;
    uint8_t *sig_tbl;
    int *cand_buf;

    for(i = 0 ; i < num_output; i++) {
        cand_cnt = 0;

        output = (int8_t*)output_addr[i];
        src = output + coord_len;
        sig_tbl = sig_tbl_addr[i];
        cand_buf = cand_buf_addr[i];

        for (j = 0; j < num_grids[i*2 + 0] * num_grids[i*2 + 1]; j++) {
            for (k = 0; k < num_anchor; k++) {
                uint8_t obj = sig_tbl[src[k*box_len]];

                if (obj >= th_conf) {
                    if ((cand_cnt + 1) > MAX_CANDIDATE_BOX) {
                        enlight_log("candidate box count overflow %d\n", cand_cnt);
                        break;
                    }
                    cand_buf[cand_cnt++] = num_anchor * j + k;
                }
            }

            src += aligned_ch;
        }
        cand_nums[i] = cand_cnt;
    }
}

//#define DEBUG_DECOMPOSE
static void decompose_output(
    void** output_addr, void** prior_box_addr, uint8_t** sig_tbl_addr,
    int num_anchor, int num_class, int num_output,
    int** cand_buf_addr, int* cand_nums,
    tensor_t *loc, tensor_t *conf, tensor_t *prior, tensor_t *obj)
{
    int i, j, k;

    //  ouput tensor shape
    //      output[h * w][box_data_size * 5 + 0's padding] = ouput[13 * 13][128]
    const int word = 32;
    const int coordinate_len = 4; //xywh
    const int ch_box_num = num_anchor;
    const int obj_len = 1;

    int box_len, ch, aligned_ch;
    int8_t *coor_dst, *conf_dst, *oact_base;
    uint8_t *obj_dst, *prior_dst, *sig_tbl;

    int* cand_buf;
    int cand_num;

    // select default box, which conf > threshould_conf
    for(i = 0 ; i < num_output; i++) {
        prior_dst = prior[i].buf;
        cand_num = cand_nums[i]; 
        cand_buf = cand_buf_addr[i];

        for(j = 0 ; j < cand_num; j++) {
            uint8_t *src = (uint8_t *)prior_box_addr[i] + (coordinate_len * cand_buf[j]);

            for(k = 0; k < coordinate_len; k++)
                *prior_dst++ = *src++;
        }
    }

    // copy loc, obj, class conf
    box_len = coordinate_len + obj_len + num_class;
    ch = ch_box_num * box_len;
    aligned_ch = (ch + (word - 1)) & ~ (word - 1);

    for(i = 0 ; i < num_output; i++) {

        coor_dst = loc[i].buf;
        obj_dst = obj[i].buf;
        conf_dst = conf[i].buf;
        oact_base = (int8_t*)output_addr[i];

        cand_num = cand_nums[i];
        cand_buf = cand_buf_addr[i];
        sig_tbl = sig_tbl_addr[i];

#ifdef DUMP
        if (cand_num > 0) {
            int8_t *obj_original =(int8_t*)enlight_malloc(cand_num);
            for(j = 0 ; j < cand_num; j++) {
                int grid_idx, box_idx;
                int8_t *obj_src;

                grid_idx = cand_buf[j] / num_anchor;
                box_idx = cand_buf[j] % num_anchor;
                obj_src = oact_base + (aligned_ch * grid_idx) + (box_len * box_idx) + coordinate_len;
                obj_original[j] = *obj_src;
            }

            FILE *fp;
            char dump_name[256];
            sprintf(dump_name, "cand_obj_%d.bin", i);
            fp = fopen(dump_name, "wb");
            fwrite(obj_original, 1, cand_num, fp);
            fclose(fp);
            enlight_free((uint8_t*)obj_original);
        }
#endif

        for(j = 0 ; j < cand_num; j++) {
            int grid_idx, box_idx;
            int8_t *coor_src, *obj_src, *conf_src;

            grid_idx = cand_buf[j] / num_anchor;
            box_idx = cand_buf[j] % num_anchor;

            coor_src = oact_base + (aligned_ch * grid_idx) + (box_len * box_idx);
            obj_src = coor_src + coordinate_len;
            conf_src = obj_src + obj_len;

            for(k = 0; k < coordinate_len; k++)
                *coor_dst++ = *coor_src++;

            *obj_dst++ = sig_tbl[*obj_src];

            for(k = 0; k < num_class; k++)
                *conf_dst++ = *conf_src++;
        }

#ifdef DEBUG_DECOMPOSE
        enlight_log("Prior\n");
        for(j = 0 ; j < cand_num * 4; j++) {
            if ((j % 4) == 0)
                enlight_log("  ");
            enlight_log(" %02x", ((uint8_t*)prior[i].buf)[j]);
        }
        enlight_log("\n");

        enlight_log("Coordinate\n");
        for(j = 0 ; j < cand_num * 4; j++) {
            if ((j % 4) == 0)
                enlight_log("  ");
            enlight_log(" %02x", ((uint8_t*)loc[i].buf)[j]);
        }
        enlight_log("\n");

        enlight_log("Objectness\n");
        for(j = 0 ; j < cand_num; j++) {
            enlight_log(" %4d", ((uint8_t*)obj[i].buf)[j]);
        }
        enlight_log("\n");

        enlight_log("Class Conf\n");
        for(j = 0 ; j < cand_num * num_class; j++) {
            if ((j % num_class) == 0)
                enlight_log("\n");
            enlight_log(" %4d", ((int8_t*)conf[i].buf)[j]);
        }
        enlight_log("\n");
#endif
    }
}

static int yolo_detector(void** prior_box_addr, void** output_addr,
             uint32_t** out_exp_tbl_addr, uint8_t** out_sig_tbl_addr,
             int pri_scl, int* output_scales, int softmax_use,
             int num_class, int num_anchor, int* num_grids, int num_output,
             int* img_size,
             int th_conf, int th_iou)
{
    int i;
    volatile unsigned long cycle0, cycle1, cycle0_h, cycle1_h;

    tensor_t scr[3];
    tensor_t loc[3];
    tensor_t pri[3];
    tensor_t obj[3];

    int cand_size = 0;
    int cand_nums[3];
    int* cand_buf_addr[3];
    int* cand_buf = (int*)enlight_malloc(MAX_CANDIDATE_BOX);

    for(i = 0; i < num_output; i++) {
        cand_buf_addr[i] = cand_buf + cand_size;
        cand_size += num_grids[i*2 + 0] * num_grids[i*2 + 1];
    }

    uint8_t  *out_sig_tbl;
    uint8_t  *sig_tbl_addr[3];
    uint32_t *out_exp_tbl;
    uint32_t *exp_tbl_addr[3];
    for(i = 0; i < num_output; i++) {
        out_sig_tbl = out_sig_tbl_addr[i];
        out_exp_tbl = out_exp_tbl_addr[i];
        sig_tbl_addr[i] = &out_sig_tbl[128];
        exp_tbl_addr[i] = &out_exp_tbl[128];
    }

    get_candidate(output_addr, cand_buf_addr, cand_nums, sig_tbl_addr,
                  num_output, num_grids, num_anchor, num_class, th_conf);

    void* buf_base[3];
    uint8_t *obj_buf;
    uint8_t *pri_buf;
    int8_t *loc_buf;
    int8_t *con_buf;
    int obj_len, pri_len, loc_len, con_len;
    int cand_num;
    int output_scl;

    for(i = 0 ; i < num_output; i++) {
        cand_num = cand_nums[i];
        output_scl = output_scales[i];

        obj_len = sizeof(uint8_t) * cand_num;
        pri_len = sizeof(uint8_t) * cand_num * 4;
        loc_len = sizeof(int8_t)  * cand_num * 4;
        con_len = sizeof(int8_t)  * cand_num * num_class;

        buf_base[i] = enlight_malloc(obj_len + pri_len + loc_len + con_len);
        out_sig_tbl = sig_tbl_addr[i];
        out_exp_tbl = exp_tbl_addr[i];

        obj_buf = (uint8_t*)buf_base[i];
        pri_buf = (uint8_t*)obj_buf + obj_len;
        loc_buf = (int8_t*)pri_buf + pri_len;
        con_buf = (int8_t*)loc_buf + loc_len;

        obj[i].buf = obj_buf;
        obj[i].scl = 8;
        obj[i].dtype = TYPE_UINT8;
        obj[i].num_ele = 1 * cand_num;
        obj[i].num_dim = 2;
        obj[i].dims[0] = 1;
        obj[i].dims[1] = cand_num;
        obj[i].exp_tbl = NULL;
        obj[i].sig_tbl = NULL;

        pri[i].buf = pri_buf;
        pri[i].scl = pri_scl;
        pri[i].dtype = TYPE_UINT8;
        pri[i].num_ele = cand_num * 4;
        pri[i].num_dim = 2;
        pri[i].dims[0] = cand_num;
        pri[i].dims[1] = 4;
        pri[i].exp_tbl = NULL;
        pri[i].sig_tbl = NULL;

        loc[i].buf = loc_buf;
        loc[i].scl = output_scl;
        loc[i].dtype = TYPE_INT8;
        loc[i].num_ele = cand_num * 4;
        loc[i].num_dim = 3;
        loc[i].dims[0] = 1;
        loc[i].dims[1] = cand_num;
        loc[i].dims[2] = 4;
        loc[i].exp_tbl = out_exp_tbl;
        loc[i].sig_tbl = out_sig_tbl;

        scr[i].buf = con_buf;
        scr[i].scl = output_scl;
        scr[i].dtype = TYPE_INT8;
        scr[i].num_ele = cand_num * num_class;
        scr[i].num_dim = 3;
        scr[i].dims[0] = 1;
        scr[i].dims[1] = cand_num;
        scr[i].dims[2] = num_class;
        scr[i].exp_tbl = out_exp_tbl;
        scr[i].sig_tbl = out_sig_tbl;
    }

    decompose_output(output_addr, prior_box_addr, sig_tbl_addr,
                     num_anchor, num_class, num_output,
                     cand_buf_addr, cand_nums,
                     loc, scr, pri, obj);

    enlight_free((uint8_t*)cand_buf);
#ifdef DUMP
    FILE *fp;
    char dump_name[256];
    for(i = 0; i < num_output; i++) {
        cand_num = cand_nums[i];
        if (cand_num > 0) {
            sprintf(dump_name, "cand_anchor_%d.bin", i);
            fp = fopen(dump_name, "wb");
            fwrite(pri[i].buf, 1, 4*cand_num, fp);
            fclose(fp);

            sprintf(dump_name, "cand_loc_%d.bin", i);
            fp = fopen(dump_name, "wb");
            fwrite(loc[i].buf, 1, 4*cand_num, fp);
            fclose(fp);

            sprintf(dump_name, "cand_scr_%d.bin", i);
            fp = fopen(dump_name, "wb");
            fwrite(scr[i].buf, 1, 20*cand_num, fp);
            fclose(fp);

            sprintf(dump_name, "cand_obj_after_sigmoid_%d.bin", i);
            fp = fopen(dump_name, "wb");
            fwrite(obj[i].buf, 1, 1*cand_num, fp);
            fclose(fp);
        }
    }
#endif


#ifdef DETECTOR_DEBUG
    for(i = 0 ; i < num_output; i++) {
        enlight_log(" pri.scl      %08x\n",     pri[i].scl);
        enlight_log(" pri.dtype    %08x\n",     pri[i].dtype);
        enlight_log(" pri.num_ele  %08x\n",     pri[i].num_ele);
        enlight_log(" pri.num_dim  %08x\n",     pri[i].num_dim);
        enlight_log(" pri.dims[0]  %08x\n",     pri[i].dims[0]);
        enlight_log(" pri.dims[1]  %08x\n",     pri[i].dims[1]);
        enlight_log("\n");
        enlight_log(" loc.buf      %08x\n",     loc[i].buf );
        enlight_log(" loc.scl      %08x\n",     loc[i].scl);
        enlight_log(" loc.dtype    %08x\n",     loc[i].dtype);
        enlight_log(" loc.num_ele  %08x\n",     loc[i].num_ele);
        enlight_log(" loc.num_dim  %08x\n",     loc[i].num_dim);
        enlight_log(" loc.dims[0]  %08x\n",     loc[i].dims[0]);
        enlight_log(" loc.dims[1]  %08x\n",     loc[i].dims[1]);
        enlight_log(" loc.dims[2]  %08x\n",     loc[i].dims[2]);
        enlight_log("\n");
        enlight_log(" scr.buf      %08x\n",     scr[i].buf);
        enlight_log(" scr.scl      %08x\n",     scr[i].scl);
        enlight_log(" scr.dtype    %08x\n",     scr[i].dtype);
        enlight_log(" scr.num_ele  %08x\n",     scr[i].num_ele);
        enlight_log(" scr.num_dim  %08x\n",     scr[i].num_dim);
        enlight_log(" scr.dims[0]  %08x\n",     scr[i].dims[0]);
        enlight_log(" scr.dims[1]  %08x\n",     scr[i].dims[1]);
        enlight_log(" scr.dims[2]  %08x\n",     scr[i].dims[2]);
    }
#endif

#ifdef __i386__
    asm volatile ("rdtsc" : "=a"(cycle0), "=d"(cycle0_h));
#else
    asm volatile ("rdcycle %0" : "=r" (cycle0));
#endif

    detection_t* r1 = yolo_post_process(scr, loc, pri, obj, th_conf, th_iou, num_grids, num_output, softmax_use);

#ifdef __i386__
    asm volatile ("rdtsc" : "=a"(cycle1), "=d"(cycle1_h));
#else
    asm volatile ("rdcycle %0" : "=r" (cycle1));
#endif
    enlight_log("[Postproc cycles] %ld \n", cycle1 - cycle0);

    enlight_log("%d object is detected\n", r1->n);

    objects.cnt = 0;

#ifdef DUMP
    
    if (r1->n > 0) {
        uint16_t *box_write = (uint16_t*)enlight_malloc(2 * 4 * r1->n);
        for(i = 0; i < r1->n; i++)
        {
            box_t* b = r1->box + i;
            box_write[4*i] = b->x_min;
            box_write[4*i+1] = b->y_min;
            box_write[4*i+2] = b->x_max;
            box_write[4*i+3] = b->y_max;
        }
        fp = fopen("box_info.bin", "wb");
        fwrite(box_write, 2, 4 * r1->n, fp);
        fclose(fp);
        enlight_free((uint8_t*)box_write);
    }
#endif

    for (i = 0 ; i < r1->n ; i++) {
        box_t* b = r1->box + i;

        enlight_log("x_min: %d, x_max: %d, y_min: %d, y_max: %d, score: %d, class: %d\n",
                img_size[0]*b->x_min/256,
                img_size[0]*b->x_max/256,
                img_size[1]*b->y_min/256,
                img_size[1]*b->y_max/256,
                ((int)r1->score[i])*100/256,
                r1->class[i]);

        if ((objects.cnt + 1) > MAX_DETECT_BOX) {
            enlight_log("object num overflow %d\n", objects.cnt);
            break;
        }

        obj_t *detect_obj = &objects.obj[objects.cnt];
        detect_obj->x_min = img_size[0]*b->x_min/256;
        detect_obj->x_max = img_size[0]*b->x_max/256;
        detect_obj->y_min = img_size[1]*b->y_min/256;
        detect_obj->y_max = img_size[1]*b->y_max/256;
        detect_obj->class = r1->score[i];
        detect_obj->score = r1->class[i] + 1; // + 1 for background
        detect_obj->img_w = img_size[0];
        detect_obj->img_h = img_size[1];
        objects.cnt++;
    }

    enlight_free((uint8_t*)r1);
    for(i = 0 ; i < num_output; i++)
        enlight_free(buf_base[i]);

    return 0;
}

#ifdef SUPPORT_MALLOC
void yolo_init_detector()
{
    objects.cnt = 0;
}
#else
void yolo_init_detector(void *work_buf)
{
    objects.cnt = 0;
    enlight_init_mem(work_buf);
}
#endif

void yolo_run_detector(
    void** output_addr, void** prior_box_addr,
    uint32_t** out_exp_tbl_addr, uint8_t** out_sig_tbl_addr,
    int* output_scales, int pri_scl, int softmax_use,
    int num_class, int num_anchor, int* num_grids, int num_output,
    int* img_size,
    int th_conf, int th_iou)
{
    yolo_detector(prior_box_addr, output_addr,
             out_exp_tbl_addr, out_sig_tbl_addr,
             pri_scl, output_scales, softmax_use,
             num_class, num_anchor, num_grids, num_output,
             img_size,
             th_conf, th_iou);
}

