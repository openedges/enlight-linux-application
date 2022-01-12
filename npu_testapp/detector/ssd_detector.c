#include "enlight_data_type.h"
#include "enlight_network.h"
#include "detector.h"

#ifdef DUMP
#   include <stdio.h>
#endif

#ifdef GDB_FW_DEBUG
objs_t gdb_fw_objects;
#endif

static void build_box_info(box_t* box, tensor_t* loc, tensor_t* prior, int idx, int stride)
{
    enlight_detector_dbg_fin();

    int loc_idx = idx * stride;
    int box_idx = idx << 2;

    int8_t xywh[4];
    uint8_t* _prior = (uint8_t*)prior->buf + box_idx;

    uint32_t* table = (uint32_t*)loc->exp_tbl;

    int x, y, w, h, x_center, y_center, x_offset, y_offset;

    const float cal_scl = 256.;
    const float tbl_scl = 256.;

    int y_off = idx;
    int x_off = 0;
    int c_off = 0;

    xywh[0] = enlight_get_tensor_qdata_by_index(loc->output_tensor, y_off, x_off, c_off);
    xywh[1] = enlight_get_tensor_qdata_by_index(loc->output_tensor, y_off, x_off, c_off + 1);
    xywh[2] = enlight_get_tensor_qdata_by_index(loc->output_tensor, y_off, x_off, c_off + 2);
    xywh[3] = enlight_get_tensor_qdata_by_index(loc->output_tensor, y_off, x_off, c_off + 3);

    w = (int)_prior[2] * table[xywh[2]];
    h = (int)_prior[3] * table[xywh[3]];

    enlight_detector_dbg("loc %8d %8d %8d %8d\n",
        xywh[0], xywh[1], xywh[2], xywh[3]);

    enlight_detector_dbg("exp(loc) %8d %8d %8d %8d\n",
        table[xywh[0]], table[xywh[1]], table[xywh[2]], table[xywh[3]]);

    w = w * cal_scl / (tbl_scl * prior->scl);
    h = h * cal_scl / (tbl_scl * prior->scl);

    enlight_detector_dbg("scale %8f %8f %8f\n", prior->scl, tbl_scl, cal_scl);
    enlight_detector_dbg("w h %8d %8d\n", w, h);

    // x_offset, y_offset scale
    // S_xoff = S_loc * S_prior
    x_offset = (int)xywh[0] * (int)_prior[2];
    y_offset = (int)xywh[1] * (int)_prior[3];

    x_offset = x_offset * cal_scl / (loc->scl * prior->scl);
    y_offset = y_offset * cal_scl / (loc->scl * prior->scl);

    x_center = _prior[0];
    y_center = _prior[1];

    if (cal_scl != prior->scl) {
        x_center = x_center * cal_scl / prior->scl;
        y_center = y_center * cal_scl / prior->scl;
    }

    x = x_center - w / 2 + x_offset;
    y = y_center - h / 2 + y_offset;

    box->x_min = x;
    box->x_max = x + w;
    box->y_min = y;
    box->y_max = y + h;

    enlight_detector_dbg("min xy(%3d, %3d) max xy(%3d, %3d)\n",
        x, y, x + w, y + h);

    enlight_detector_dbg_fout();
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


static void thresholding_with_candidate(
    tensor_t* score,
    cand_t* cand,
    tensor_t* loc,
    tensor_t* prior,
    int *cand_box,
    int num_cand_box,
    int class,
    float threshold,
    int stride)
{
    enlight_detector_dbg_fin();

    uint32_t* score_buf_base = (uint32_t*)score->buf;
    uint32_t* score_buf = &score_buf_base[class];

    int num_class = score->dims[2];

    int i, box_idx;
    int n = 0;

    for (i = 0; i < num_cand_box; i++) {
        box_idx = cand_box[i];

        if (score_buf[num_class * i] >= ((threshold * 256 * 256))) {

            enlight_detector_dbg("score:thres = %8d:%8d",
                score_buf[num_class * i],
                ((int)(threshold * 256 * 256)));

            build_box_info(&cand->box[n], loc, prior, box_idx, stride);
            cand->class[n] = class;
            cand->score[n] = score_buf[num_class * i];
            n++;
        }
    }
    cand->n = n;

    enlight_detector_dbg_fout();
}

#define MAX_NMS_CAND 256

detection_t* ssd_post_process(tensor_t* score, tensor_t* loc, tensor_t* pri,
                              float th_conf, float th_nms, const int word_size)
{
    enlight_detector_dbg_fin();

    detection_t* detection = alloc_detection(score->num_ele);

    // batches, boxes, classes
    int len_loc = loc->dims[2];
    int num_box = score->dims[1];
    int num_cls = score->dims[2];

    int conf_stride = (num_cls + (word_size - 1)) & ~ (word_size - 1);
    int loc_stride = (len_loc + (word_size - 1)) & ~ (word_size - 1);

    int class;

    int num_cand_box;
    int *cand_box = (int*)enlight_malloc(4 * MAX_NMS_CAND);

    tensor_t* cand_conf;
    int dst_dims[3];
    int i;

    for (i = 0; i < score->num_dim; i++)
        dst_dims[i] = score->dims[i];
    dst_dims[1] = MAX_NMS_CAND;

    cand_conf = alloc_tensor(ENLIGHT_DTYPE_UINT32, score->num_dim, dst_dims, 0);

    num_cand_box = softmax_with_threshold(score, cand_conf, cand_box,
                           conf_stride, th_conf, MAX_NMS_CAND);
#ifdef DUMP
    {
        FILE* fp = fopen("regression_result.bin", "wb");
        fprintf(cand_conf->buf, 1, 4 * num_cand_box * num_cls, fp);
        fclose(fp);
    }
#endif

    for (class = 1 ; class < num_cls ; class++)
    {
        int num_box = cand_conf->dims[1];
        cand_t* cand = (cand_t*)alloc_candidate(num_box);

        thresholding_with_candidate(cand_conf, cand, loc, pri, cand_box,
                                    num_cand_box, class, th_conf, loc_stride);

#ifdef DUMP
        {
            static FILE* fp = NULL;

            if (fp == NULL)
                fp = fopen("cand_decoded_box.log", "w");

            if(cand->n > 0) {
                int i;

                for(i = 0; i < num_cand; i++) {
                    box_t* c_box = &cand->box[i];
                    fprintf(fp, "class %d: box[%2d] x_min y_min x_max_y_max : %4d %4d %4d %4d",
                        class, i, c_box->x_min, c_box->y_min, c_box->x_max, c_box->y_max);
                }
                fclose(fp);
            }
        }
#endif

        nms(detection, cand, class, th_nms);

        free_candidate(cand);
    }

    enlight_free(cand_box);
    free_tensor(cand_conf);

    enlight_detector_dbg_fout();
    return detection;
}


void ssd_init()
{
#ifndef SUPPORT_MALLOC
    extern uint8_t* detector_work_buf;
    enlight_init_mem(detector_work_buf);
#endif
}
/** @brief SSD detector wrapper function
 */
int ssd_detect_object(
    int           num_loc,                  /**< output loc tensor number    */
    int           num_conf,                 /**< output conf tensor number   */
    act_tensor_t** loc_tensors,             /**< loc output tensors          */
    act_tensor_t** conf_tensors,            /**< conf output tnesors         */
    uint8_t*      default_box_addrs,        /**< default box base addresses  */
    float         default_box_scales,       /**< default box scales          */
    uint32_t*     loc_exp_tables,           /**< conf output exp(x) tables   */
    uint32_t*     con_exp_tables,           /**< loc output exp(x) tables    */
    int           num_class,                /**< number of class             */
    int           num_box,                  /**< number of box               */
    int*          img_sizes,                /**< input image H, W            */
    float         th_conf,                  /**< confidence threshold        */
    float         th_iou,                   /**< IOU threshold               */
    objs_t*       detected_objs             /**< return objects buf base     */
)
{
    enlight_detector_dbg_fin();

    const int word_size = 32;
    int i;

#ifdef POST_PROCESS_REPORT_CYCLE
    volatile unsigned long cycle0, cycle1, cycle0_h, cycle1_h;
#endif //POST_PROCESS_REPORT_CYCLE

    tensor_t scr, loc, pri;

    enlight_detector_dbg("num_loc                  %8d\n", num_loc);
    enlight_detector_dbg("num_conf                 %8d\n", num_conf);

    for (i = 0; i < num_loc; i++)
        enlight_detector_dbg("loc tensor addr          %08x\n", loc_tensors[i]);

    for (i = 0; i < num_conf; i++)
        enlight_detector_dbg("conf tensor addr         %08x\n", conf_tensors[i]);

    enlight_detector_dbg("default box addr         %08x\n", default_box_addrs);
    enlight_detector_dbg("default_box_scales       %f\n",  default_box_scales);

    enlight_detector_dbg("loc_exp_tables addr      %08x\n", loc_exp_tables);
    enlight_detector_dbg("con_exp_tables addr      %08x\n", con_exp_tables);

    enlight_detector_dbg("num_class                %8d\n", num_class);
    enlight_detector_dbg("num_box                  %8d\n", num_box);
    enlight_detector_dbg("img_sizes  H             %8d\n", img_sizes[0]);
    enlight_detector_dbg("img_sizes  W             %8d\n", img_sizes[1]);

    enlight_detector_dbg("th_conf                  %f\n", th_conf);
    enlight_detector_dbg("th_iou                   %f\n", th_iou);
    enlight_detector_dbg("\n");

    pri.output_tensor = NULL;
    pri.buf      = default_box_addrs;
    pri.scl      = default_box_scales;
    pri.dtype    = ENLIGHT_DTYPE_UINT8;
    pri.num_ele  = num_box*4;
    pri.num_dim  = 2;
    pri.dims[0]  = num_box;
    pri.dims[1]  = 4;
    pri.exp_tbl  = NULL;

    loc.output_tensor = (void*)loc_tensors[0];
    loc.buf     = NULL;
    loc.scl     = enlight_get_tensor_scale(loc_tensors[0]);
    loc.dtype   = ENLIGHT_DTYPE_INT8;
    loc.num_ele = 1*num_box*4;
    loc.num_dim = 3;
    loc.dims[0] = 1;
    loc.dims[1] = num_box;
    loc.dims[2] = 4;
    loc.exp_tbl = &loc_exp_tables[128];

    scr.output_tensor = (void*)conf_tensors[0];
    scr.buf     = NULL;
    scr.scl     = enlight_get_tensor_scale(conf_tensors[0]);
    scr.dtype   = ENLIGHT_DTYPE_INT8;
    scr.num_ele = 1*num_box*num_class;
    scr.num_dim = 3;
    scr.dims[0] = 1;
    scr.dims[1] = num_box;
    scr.dims[2] = num_class;
    scr.exp_tbl = &con_exp_tables[128];

#ifdef DETECTOR_DEBUG
    enlight_detector_dbg(" pri.buf        0x%08x\n", pri.buf );
    enlight_detector_dbg(" pri.scl          %8f\n", pri.scl);
    enlight_detector_dbg(" pri.dtype        %8d\n", pri.dtype);
    enlight_detector_dbg(" pri.num_ele      %8d\n", pri.num_ele);
    enlight_detector_dbg(" pri.num_dim      %8d\n", pri.num_dim);
    enlight_detector_dbg(" pri.dims[0]      %8d\n", pri.dims[0]);
    enlight_detector_dbg(" pri.dims[1]      %8d\n", pri.dims[1]);
    enlight_detector_dbg(" pri.table      0x%08x\n", pri.exp_tbl);
    enlight_detector_dbg("\n");
    enlight_detector_dbg(" loc.output_tensor      0x%08x\n", loc.output_tensor);
    enlight_detector_dbg(" loc.buf        0x%08x\n", loc.buf );
    enlight_detector_dbg(" loc.scl          %8f\n",  loc.scl);
    enlight_detector_dbg(" loc.dtype        %8d\n", loc.dtype);
    enlight_detector_dbg(" loc.num_ele      %8d\n", loc.num_ele);
    enlight_detector_dbg(" loc.num_dim      %8d\n", loc.num_dim);
    enlight_detector_dbg(" loc.dims[0]      %8d\n", loc.dims[0]);
    enlight_detector_dbg(" loc.dims[1]      %8d\n", loc.dims[1]);
    enlight_detector_dbg(" loc.dims[2]      %8d\n", loc.dims[2]);
    enlight_detector_dbg(" loc.table      0x%08x\n", loc.exp_tbl);
    enlight_detector_dbg("\n");
    enlight_detector_dbg(" scr.output_tensor      0x%08x\n", scr.output_tensor);
    enlight_detector_dbg(" scr.buf        0x%08x\n", scr.buf);
    enlight_detector_dbg(" scr.scl          %8f\n", scr.scl);
    enlight_detector_dbg(" scr.dtype        %8d\n", scr.dtype);
    enlight_detector_dbg(" scr.num_ele      %8d\n", scr.num_ele);
    enlight_detector_dbg(" scr.num_dim      %8d\n", scr.num_dim);
    enlight_detector_dbg(" scr.dims[0]      %8d\n", scr.dims[0]);
    enlight_detector_dbg(" scr.dims[1]      %8d\n", scr.dims[1]);
    enlight_detector_dbg(" scr.dims[2]      %8d\n", scr.dims[2]);
    enlight_detector_dbg(" scr.table      0x%08x\n", scr.exp_tbl);
#endif

#ifdef POST_PROCESS_REPORT_CYCLE
#ifdef __i386__
    asm volatile ("rdtsc" : "=a"(cycle0), "=d"(cycle0_h));
#else
    asm volatile ("rdcycle %0" : "=r" (cycle0));
#endif
#endif //POST_PROCESS_REPORT_CYCLE

    detection_t* r1 = ssd_post_process(&scr, &loc, &pri, th_conf, th_iou, word_size);

#ifdef POST_PROCESS_REPORT_CYCLE
#ifdef __i386__
    asm volatile ("rdtsc" : "=a"(cycle1), "=d"(cycle1_h));
#else
    asm volatile ("rdcycle %0" : "=r" (cycle1));
#endif
    enlight_detector_dbg("[Postproc cycles] %ld \n", cycle1 - cycle0);
#endif //POST_PROCESS_REPORT_CYCLE
    
    enlight_detector_dbg("%d object is detected\n", r1->n);

    for (i = 0; i < r1->n ; i++) {
        box_t* b = r1->box + i;

        enlight_detector_dbg("x_min: %d, x_max: %d, y_min: %d, y_max: %d, score: %d%, class: %d\n",
                img_sizes[1]*b->x_min/256,
                img_sizes[1]*b->x_max/256,
                img_sizes[0]*b->y_min/256,
                img_sizes[0]*b->y_max/256,
                r1->score[i]*100/(1<<16),
                r1->class[i]);

#ifdef GDB_FW_DEBUG
        obj_t *dbg_obj = &gdb_fw_objects.obj[i];
        dbg_obj->x_min = img_sizes[1]*b->x_min/256;
        dbg_obj->x_max = img_sizes[1]*b->x_max/256;
        dbg_obj->y_min = img_sizes[0]*b->y_min/256;
        dbg_obj->y_max = img_sizes[0]*b->y_max/256;
        dbg_obj->score = r1->score[i]*100/(1<<16);
        dbg_obj->class = r1->class[i];
        dbg_obj->img_w = img_sizes[1];
        dbg_obj->img_h = img_sizes[0];
        gdb_fw_objects.cnt = r1->n;
#endif

        obj_t *ret_obj = &detected_objs->obj[i];
        ret_obj->x_min = img_sizes[1]*b->x_min/256;
        ret_obj->x_max = img_sizes[1]*b->x_max/256;
        ret_obj->y_min = img_sizes[0]*b->y_min/256;
        ret_obj->y_max = img_sizes[0]*b->y_max/256;
        ret_obj->score = r1->score[i]*100/(1<<16);
        ret_obj->class = r1->class[i];
        ret_obj->img_w = img_sizes[1];
        ret_obj->img_h = img_sizes[0];
        detected_objs->cnt = r1->n;
    }

    enlight_free(r1);

    enlight_detector_dbg_fout();
    return 0;
}

