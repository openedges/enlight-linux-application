#include "enlight_data_type.h"
#include "enlight_network.h"
#include "detector.h"

#ifdef DUMP
#   include <stdio.h>
#endif

#ifdef GDB_FW_DEBUG
extern objs_t gdb_fw_objects;
#endif

static void build_box_info(
    box_t*      box,
    tensor_t*   loc,
    tensor_t*   prior,
    int         idx,
    int*        num_grid)
{
    enlight_detector_dbg_fin();

    enlight_detector_dbg("idx:          %d\n", idx);
    enlight_detector_dbg("prior base:   0x%08x\n", prior);
    enlight_detector_dbg("num_grid:     %d %d\n", num_grid[0], num_grid[1]);

    int loc_idx = idx * 4;
    int box_idx = idx * 4;

    int8_t* _loc = (int8_t*)loc->buf + loc_idx;
    uint32_t* _prior = (uint32_t*)prior->buf + box_idx;

    enlight_detector_dbg("loc[0 ~ 3]: %3d %3d %3d %3d\n",
        _loc[0],_loc[1], _loc[2], _loc[3]);

    enlight_detector_dbg("prior[0 ~ 3]: 0x%02x 0x%02x 0x%02x 0x%02x\n",
        _prior[0],_prior[1], _prior[2], _prior[3]);

    uint32_t* sig_tbl = (uint32_t*)loc->sig_tbl;
    uint32_t* exp_tbl = (uint32_t*)loc->exp_tbl;

    int x, y, w, h, anchor_x, anchor_y;

    x = (int)sig_tbl[_loc[0]];
    y = (int)sig_tbl[_loc[1]];

    // (x / num_grid)
    // (y / num_grid)
    uint32_t compensation_bit = 16;
    uint32_t compensation_x = (1 << compensation_bit) / num_grid[0];
    uint32_t compensation_y = (1 << compensation_bit) / num_grid[1];
    uint32_t temp;

    temp = (uint32_t)x * compensation_x;
    x = (int)(temp >> compensation_bit);
    temp = (uint32_t)y * compensation_y;
    y = (int)(temp >> compensation_bit);

    const int result_scl = 256;
    const int sigmoid_scl = 256;
    const int exp_scl = 256;

    x = (x * result_scl) / sigmoid_scl;
    y = (y * result_scl) / sigmoid_scl;

    anchor_x = ((int)_prior[0]) * result_scl / prior->scl;
    anchor_y = ((int)_prior[1]) * result_scl / prior->scl;

    x = x + anchor_x;
    y = y + anchor_y;

    w = (int)_prior[2] * exp_tbl[_loc[2]];
    h = (int)_prior[3] * exp_tbl[_loc[3]];

    w = (w * result_scl) / (exp_scl * prior->scl);
    h = (h * result_scl) / (exp_scl * prior->scl);

    w >>= 1;
    h >>= 1;

    box->x_min = x - w;
    box->x_max = x + w;
    box->y_min = y - h;
    box->y_max = y + h;

    enlight_detector_dbg("x %4d %4d y %4d %4d: w%4d h%4d\n",
        box->x_min, box->x_max, box->y_min, box->y_max, w, h);

    enlight_detector_dbg_fout();
}


// input: { batch, boxes, classes }
// output: { boxes { coordinate, score } }
static void thresholding(tensor_t** score, cand_t* cand,
                         tensor_t* loc, tensor_t* prior,
                         int class, float th, int* num_grids, int num_output)
{
    enlight_detector_dbg_fin();

    uint32_t* _score;
    int num_box;
    int num_class = score[0]->dims[2];

    int i, box;
    int n = 0;

    // enlight_detector_dbg("class:            %d\n", class);
    // enlight_detector_dbg("th * 1024:        %d\n", (int)(th * 256));
    // enlight_detector_dbg("num_grids:        %d %d %d %d %d %d\n", 
    //     num_grids[0], num_grids[1],
    //     num_grids[2], num_grids[3],
    //     num_grids[4], num_grids[5]);
    // enlight_detector_dbg("num_output:       %d\n", num_output);

    for (i = 0; i < num_output; i++) {
        _score = (uint32_t*)score[i]->buf + class;
        num_box = score[i]->dims[1];

        for (box = 0; box < num_box; box++, _score += num_class) {
            if (*_score >= ((int)(th * 256 * 256))) {

                enlight_detector_dbg("score:thres = %8d:%8d", *_score, ((int)(th * 256 * 256)));

                build_box_info(&cand->box[n], &loc[i], &prior[i], box, &num_grids[i*2]);
                cand->class[n] = class;
                cand->score[n] = *_score;
                n++;
            }
        }
    }
    cand->n = n;

    enlight_detector_dbg_fout();
}


static detection_t* yolo_post_process(
    tensor_t*   score,
    tensor_t*   loc,
    tensor_t*   pri,
    tensor_t*   obj,
    float       th_conf,
    float       th_nms,
    int*        num_grids,
    int         num_output,
    int         softmax_use
)
{
    enlight_detector_dbg_fin();

    // enlight_detector_dbg("th_conf * 1024:    %8d\n", (int)(1024 * th_conf));
    // enlight_detector_dbg("th_nms  * 1024:    %8d\n", (int)(1024 * th_nms));
    // enlight_detector_dbg("num_grid      :    %8d,%8d,%8d,%8d,%8d,%8d\n",
    //     num_grids[0], num_grids[1], num_grids[2], num_grids[3],
    //     num_grids[4], num_grids[5]);
    // enlight_detector_dbg("softmax_use   :    %8d\n", softmax_use);

    detection_t* detection;
    int i, j, c;
    int num_cls, num_box;
    int total_num_box = 0;
    uint32_t *conf_buf;
    uint32_t *obj_buf;
    tensor_t* conf[3];

    int num_ele = 0;
    for(i = 0; i < num_output; i++)
        num_ele += score[i].num_ele;

    detection = alloc_detection(num_ele);
    num_cls = score[0].dims[2];

    for(i =0; i < num_output; i++) {
        num_box = score[i].dims[1];
        total_num_box += num_box;

        conf[i] = alloc_tensor(ENLIGHT_DTYPE_UINT32, score[i].num_dim, score[i].dims, 0);

        if (softmax_use)
            softmax(&score[i], conf[i], score[i].dims[2]);
        else
            sigmoid(&score[i], conf[i], score[i].dims[2]);

        conf_buf = conf[i]->buf;
        obj_buf = (uint32_t*)obj[i].buf;

        for(j = 0; j < num_box; j++) {

            for(c = 0; c < num_cls; c++) {
                uint32_t conf_val = *conf_buf;
                uint32_t obj_val = *obj_buf;
                uint32_t conf_obj;

                conf_obj = conf_val * obj_val;

                // enlight_detector_dbg("[box %2d cls %4d] %8d * %8d = %8d\n",
                //     j, c, conf_val, obj_val, conf_obj);

                *conf_buf = conf_obj; 

                conf_buf++;
            }
            obj_buf++;

            // enlight_detector_dbg("\n");
        }
    }

    for (c = 0; c < num_cls; c++) {
        cand_t* cand = (cand_t*)alloc_candidate(total_num_box);

        thresholding(conf, cand, loc, pri, c, th_conf, num_grids, num_output);

        {
            int i;
            for (i = 0; i < cand->n; i++) {
                box_t* box = &cand->box[i];
                int class = cand->class[i];
                int score = cand->score[i];

                enlight_detector_dbg("x%4d %4d y%4d %4d: cls %4d scr %4d\n",
                box->x_min, box->x_max, box->y_min, box->y_max,
                class, score);
            }
        }

        nms(detection, cand, c, th_nms);

        free_candidate(cand);
    }

    for(i = 0; i < num_output; i++)
        free_tensor(conf[i]);

    enlight_detector_dbg_fout();

    return detection;
}

static void get_candidate(
    act_tensor_t**  outputs,
    int**           cand_bufs,
    int*            cand_nums,
    uint32_t**      sig_tbls,
    int             num_output,
    int*            num_grids,
    int             num_anchor,
    int             num_class,
    float           th_conf
    )
{
    enlight_detector_dbg_fin();

    const int coord_len = 4;
    const int obj_len = 1;
    int box_data_len;
    int cand_cnt;
    int output_idx;

    enlight_detector_dbg("outputs:          %08x %08x %08x\n",
        outputs[0], outputs[1], outputs[2]);
    enlight_detector_dbg("num_output:       %8d\n", num_output);
    enlight_detector_dbg("num_grids:        %8d %8d %8d %8d %8d %8d\n",
        num_grids[0], num_grids[1],
        num_grids[2], num_grids[3],
        num_grids[4], num_grids[5]);
    enlight_detector_dbg("num_anchor:       %8d\n", num_anchor);
    enlight_detector_dbg("num_class:        %8d\n", num_class);
    enlight_detector_dbg("th_conf:          %8d/1000\n", (int)(th_conf * 1000));

    box_data_len = coord_len + obj_len + num_class;

#ifdef GET_CANDIDAE_DBG
    for(i = 0; i < num_output; i++) {
        uint32_t *sig_tbl;
        int gi;

        sig_tbl = sig_tbls[i];

        enlight_detector_dbg("\noutput:%02x\n", i);
        for (gi = -128; gi < 128; gi++) {
            if ((gi % 16) == 0)
                enlight_detector_dbg("\n%02x ", sig_tbl[gi]);
            else
                enlight_detector_dbg("%02x ", sig_tbl[gi]);
        }
    }
#endif

    for(output_idx = 0; output_idx < num_output; output_idx++) {
        uint32_t *sig_tbl;
        int *cand_buf;
        int grid_h, grid_w, gi, gj, k;
        cand_cnt = 0;

        sig_tbl = sig_tbls[output_idx];
        cand_buf = cand_bufs[output_idx];

        act_tensor_t* _output = outputs[output_idx];

        grid_h = num_grids[2 * output_idx];
        grid_w = num_grids[(2 * output_idx) + 1];

        for (gi = 0; gi < grid_h; gi++) {
            for (gj = 0; gj < grid_w; gj++) {
                for (k = 0; k < num_anchor; k++) {
                    const int sig_scl = 256;
                    int c_off = (box_data_len * k) + coord_len;
                    int x_off = gj;
                    int y_off = gi;

                    int8_t src = enlight_get_tensor_qdata_by_index(_output, y_off, x_off, c_off);

                    uint32_t obj = sig_tbl[src];

                    //if(k==0){
                    //    enlight_detector_dbg("\n src[%3d %3d %3d]: 0x%02x %8d",
                    //        y_off, x_off, c_off, (uint8_t)src, src);
                    //}
                    //else {
                    //    enlight_detector_dbg("    0x%02x %8d", (uint8_t)src, src);
                    //}

                    if ((obj * 1024 / sig_scl) >= ((int)(th_conf * 1024))) {

                        enlight_detector_dbg("\n src[%3d %3d %3d]: 0x%02x %8d\n",
                            y_off, x_off, c_off, (uint8_t)src, src);

                        if ((cand_cnt + 1) > MAX_CANDIDATE_BOX) {
                            enlight_detector_err("candidate box count overflow %d\n", cand_cnt);
                            break;
                        }
                        cand_buf[cand_cnt++] = num_anchor * ((grid_w * gi) + gj) + k;

                        enlight_detector_dbg("src: 0x%02x obj: 0x%02x th_conf: %8d/1000\n",
                            src, obj, (int)(th_conf * 1000));

                        enlight_detector_dbg("cand_buf[%d]: %d, %d, %d, %d, %d, %d\n",
                            (cand_cnt - 1), (num_anchor * ((grid_w * gi) + gj) + k),
                            num_anchor, grid_w, gi, gj, k);
                    }
                }
            }
        }
        cand_nums[output_idx] = cand_cnt;
        enlight_detector_dbg("cand_nums[%d]: %d\n", output_idx, cand_cnt);
    }

    enlight_detector_dbg_fout();
}

static void decompose_output(
    act_tensor_t**  outputs,
    uint8_t**       def_box_addr,
    uint32_t**      sig_tbl_addr,
    int             num_anchor,
    int             num_class,
    int             num_output,
    int*            num_grids,
    int**           cand_buf_addr,
    int*            cand_nums,
    tensor_t*       loc,
    tensor_t*       conf,
    tensor_t*       prior,
    tensor_t*       obj)
{
    enlight_detector_dbg_fin();

    int i, j, k;

    //  ouput tensor shape
    //      output[h * w][box_data_size * 5 + 0's padding] = ouput[13 * 13][128]
    const int word = 32;
    const int coordinate_len = 4; //xywh
    const int ch_box_num = num_anchor;
    const int obj_len = 1;

    // select default box, which conf > threshould_conf
    for(i = 0 ; i < num_output; i++) {
        uint32_t* prior_dst = prior[i].buf;
        int cand_num = cand_nums[i];
        int* cand_buf = cand_buf_addr[i];

        enlight_detector_dbg("cand buf base 0x%08x\n", cand_buf);
        enlight_detector_dbg("cand prior base 0x%08x\n", prior_dst);

        for(j = 0; j < cand_num; j++) {
            uint8_t *src = def_box_addr[i] + (coordinate_len * cand_buf[j]);

            enlight_detector_dbg("\nsrc 0x%08x base 0x%08x cand idx %d\n",
                src, def_box_addr[i], cand_buf[j]);

            enlight_detector_dbg("\ncand default box [output_idx:%2d][cand:%2d]", i, j);

            for(k = 0; k < coordinate_len; k++) {
                uint8_t val = *src++;
                *prior_dst++ = (uint32_t)val;

                enlight_detector_dbg("0x%02x ", val);
            }
        }
        enlight_detector_dbg("\n");
    }

    // copy loc, obj, class conf
    int box_len = coordinate_len + obj_len + num_class;
    int ch = ch_box_num * box_len;
    int aligned_ch = (ch + (word - 1)) & ~ (word - 1);

    for(i = 0 ; i < num_output; i++) {

        int8_t* coor_dst = loc[i].buf;
        uint32_t* obj_dst = obj[i].buf;
        int8_t* conf_dst = conf[i].buf;

        int cand_num = cand_nums[i];
        int* cand_buf = cand_buf_addr[i];

        uint32_t* sig_tbl = sig_tbl_addr[i];

        for(j = 0; j < cand_num; j++) {
            int grid_idx, box_idx;

            grid_idx = cand_buf[j] / num_anchor;
            box_idx = cand_buf[j] % num_anchor;

            // local
            for(k = 0; k < coordinate_len; k++) {
                int y_off = grid_idx / num_grids[2 * i];
                int x_off = grid_idx % num_grids[2 * i];
                int c_off = box_len * box_idx + k;

                int8_t val = enlight_get_tensor_qdata_by_index(outputs[i],
                                y_off, x_off, c_off);
                *coor_dst++ = val;

                if (k == 0) {
                    enlight_detector_dbg("loc[%2d,%2d,%2d]: %3d ", y_off, x_off, c_off, val);
                }
                else {
                    enlight_detector_dbg("%3d ", val);
                }
            }

            // objectness
            for(k = 0; k < 1; k++) {
                int y_off = grid_idx / num_grids[2 * i];
                int x_off = grid_idx % num_grids[2 * i];
                int c_off = box_len * box_idx + coordinate_len + k;

                uint32_t val = enlight_get_tensor_qdata_by_index(outputs[i],
                                y_off, x_off, c_off);

                val = sig_tbl[val];

                *obj_dst++ = val;

                enlight_detector_dbg("obj[%2d,%2d,%2d]:%3d ", y_off, x_off, c_off, val);
            }
            enlight_detector_dbg("\n");

            //class confidence
            for(k = 0; k < num_class; k++) {
                int y_off = grid_idx / num_grids[2 * i];
                int x_off = grid_idx % num_grids[2 * i];
                int c_off = box_len * box_idx + coordinate_len + 1 + k;

                int8_t val = enlight_get_tensor_qdata_by_index(outputs[i],
                                y_off, x_off, c_off);
                *conf_dst++ = val;

                if (k == 0) {
                    enlight_detector_dbg("cls[%2d,%2d,%2d]:%3d ", y_off, x_off, c_off, val);
                }
                else {
                    enlight_detector_dbg("%3d ", val);
                }
            }
            enlight_detector_dbg("\n");
        }

    }

    enlight_detector_dbg_fout();
}


void yolo_init()
{
    enlight_detector_dbg_fin();

#ifndef SUPPORT_MALLOC
    extern uint8_t* detector_work_buf;
    enlight_init_mem(detector_work_buf);
#endif

    enlight_detector_dbg_fout();
}

int yolo_detect_object(
    int             softmax_use,       /**< softmax in yolo2            */
    int             num_output,        /**< output loc tensor number    */
    act_tensor_t**  outputs,           /**< output conf tensor number   */
    uint8_t*        def_box_addr,      /**< default box base addresses  */
    float           def_box_scl,       /**< default box scale           */
    uint32_t*       exp_tbl_base,      /**< output exp(x) tables        */
    uint32_t*       sig_tbl_base,      /**< output sigmoid(x) tables    */
    int             num_class,         /**< class num                   */
    int*            num_grids,         /**< grids                       */
    int             num_anchor,        /**< anchor num, #box per a grid */
    int*            img_sizes,         /**< input image H, W            */
    float           th_conf,           /**< confidence threshold        */
    float           th_iou,            /**< IOU threshold               */
    objs_t*         detected_objs      /**< return objects buf base     */
    )
{
    enlight_detector_dbg_fin();

    enlight_detector_dbg("softmax_use:  %8d\n", softmax_use);
    enlight_detector_dbg("def_box_scl:  %8d / 1000\n", (int)(1000 * def_box_scl));
    enlight_detector_dbg("num_output:   %8d\n", num_output);
    enlight_detector_dbg("num_class:    %8d\n", num_class);
    enlight_detector_dbg("num_grid:     %8d,%8d,%8d,%8d,%8d,%8d\n",
        num_grids[0], num_grids[1],
        num_grids[2], num_grids[3],
        num_grids[4], num_grids[5]);
    enlight_detector_dbg("num_anchor:   %8d\n", num_anchor);
    enlight_detector_dbg("th_conf:      %8d / 1000\n", (int)(1000 * th_conf));
    enlight_detector_dbg("th_nms :      %8d / 1000\n", (int)(1000 * th_iou));

    int i;
#ifdef POST_PROCESS_REPORT_CYCLE
    volatile unsigned long cycle0, cycle1, cycle0_h, cycle1_h;
#endif //POST_PROCESS_REPORT_CYCLE

    tensor_t scr[3];
    tensor_t loc[3];
    tensor_t pri[3];
    tensor_t obj[3];
 
    int cand_size = 0;
    int cand_nums[3];
    int* cand_bufs_addr[3];
    int* cand_buf;
    int max_cand_num;

    uint32_t *sig_tbls_addr[3];
    uint32_t *exp_tbls_addr[3];
    uint8_t *def_boxs_addr[3];

    const int len_loc = 4;

#ifdef GDB_FW_DEBUG
        gdb_fw_objects.cnt = 0;
#endif

    max_cand_num = (
        (num_grids[0] * num_grids[1]) +
        (num_grids[2] * num_grids[3]) +
        (num_grids[4] * num_grids[5])) * num_anchor * sizeof(int);

    cand_buf = (int*)enlight_malloc(max_cand_num);
   
    enlight_detector_dbg("max_cand_num buf size %d\n", max_cand_num);
    
    cand_size = 0;
    for(i = 0; i < num_output; i++) {
        cand_bufs_addr[i] = &cand_buf[cand_size];
        cand_size += num_grids[i * 2] * num_grids[(i * 2) + 1];
        enlight_detector_dbg("cand_bufs_addr %08x\n", cand_bufs_addr[i]);
    }

    cand_size = 0;
    for(i = 0; i < num_output; i++) {
        enlight_detector_dbg("def_boxs_addr[%d]:0x%08x ", i, &def_box_addr[cand_size]);

        def_boxs_addr[i] = &def_box_addr[cand_size];
        cand_size += num_grids[i * 2] * num_grids[(i * 2) + 1] * num_anchor * len_loc;
   
        enlight_detector_dbg(" cand_size = %d\n", cand_size);
    }

    enlight_detector_dbg("sig_tables base: %08x\n", sig_tbl_base);
    enlight_detector_dbg("exp_tables base: %08x\n", exp_tbl_base);
    for(i = 0; i < num_output; i++) {
        sig_tbls_addr[i] = &sig_tbl_base[i * 256 + 128];
        exp_tbls_addr[i] = &exp_tbl_base[i * 256 + 128];
        enlight_detector_dbg("sig_tables[%d] addr: %08x\n", i, sig_tbls_addr[i]);
        enlight_detector_dbg("exp_tables[%d] addr: %08x\n", i, exp_tbls_addr[i]);
    }

    get_candidate(outputs, cand_bufs_addr,
                  cand_nums, sig_tbls_addr,
                  num_output, num_grids,
                  num_anchor, num_class,
                  th_conf);
    
    void* output_work_buf_base[3];

    for(i = 0 ; i < num_output; i++) {
        int cand_num = cand_nums[i];
        float output_scl = enlight_get_tensor_scale(outputs[i]);

        int obj_len = sizeof(uint32_t) * cand_num;
        int pri_len = sizeof(uint32_t) * cand_num * 4;
        int loc_len = sizeof(int8_t)  * cand_num * 4;
        int con_len = sizeof(int8_t)  * cand_num * num_class;

        output_work_buf_base[i] = enlight_malloc(obj_len + pri_len + loc_len + con_len);

        uint8_t* obj_buf = (uint8_t*)output_work_buf_base[i];
        uint8_t* pri_buf = (uint8_t*)obj_buf + obj_len;
        int8_t* loc_buf = (int8_t*)pri_buf + pri_len;
        int8_t* con_buf = (int8_t*)loc_buf + loc_len;

        obj[i].buf      = obj_buf;
        obj[i].scl      = 256*1024; //FIXME.
        obj[i].dtype    = ENLIGHT_DTYPE_UINT32;
        obj[i].num_ele  = 1 * cand_num;
        obj[i].num_dim  = 2;
        obj[i].dims[0]  = 1;
        obj[i].dims[1]  = cand_num;
        obj[i].exp_tbl  = NULL;
        obj[i].sig_tbl  = NULL;

        pri[i].buf      = pri_buf;
        pri[i].scl      = def_box_scl;
        pri[i].dtype    = ENLIGHT_DTYPE_UINT32;
        pri[i].num_ele  = cand_num * 4;
        pri[i].num_dim  = 2;
        pri[i].dims[0]  = cand_num;
        pri[i].dims[1]  = 4;
        pri[i].exp_tbl  = NULL;
        pri[i].sig_tbl  = NULL;

        loc[i].buf      = loc_buf;
        loc[i].scl      = output_scl;
        loc[i].dtype    = ENLIGHT_DTYPE_INT8;
        loc[i].num_ele  = cand_num * 4;
        loc[i].num_dim  = 3;
        loc[i].dims[0]  = 1;
        loc[i].dims[1]  = cand_num;
        loc[i].dims[2]  = 4;
        loc[i].exp_tbl  = exp_tbls_addr[i];
        loc[i].sig_tbl  = sig_tbls_addr[i];

        scr[i].buf      = con_buf;
        scr[i].scl      = output_scl;
        scr[i].dtype    = ENLIGHT_DTYPE_INT8;
        scr[i].num_ele  = cand_num * num_class;
        scr[i].num_dim  = 3;
        scr[i].dims[0]  = 1;
        scr[i].dims[1]  = cand_num;
        scr[i].dims[2]  = num_class;
        scr[i].exp_tbl  = exp_tbls_addr[i];
        scr[i].sig_tbl  = sig_tbls_addr[i];
    }

    decompose_output(outputs,
                     def_boxs_addr,
                     sig_tbls_addr,
                     num_anchor, num_class, num_output, num_grids,
                     cand_bufs_addr, cand_nums,
                     loc, scr, pri, obj);

    enlight_free(cand_buf);

    for(i = 0 ; i < num_output; i++) {
        enlight_detector_dbg(" pri.buf      %08x\n",     pri[i].buf);
        enlight_detector_dbg(" pri.scl      %08x\n",     pri[i].scl);
        enlight_detector_dbg(" pri.dtype    %08x\n",     pri[i].dtype);
        enlight_detector_dbg(" pri.num_ele  %08x\n",     pri[i].num_ele);
        enlight_detector_dbg(" pri.num_dim  %08x\n",     pri[i].num_dim);
        enlight_detector_dbg(" pri.dims[0]  %08x\n",     pri[i].dims[0]);
        enlight_detector_dbg(" pri.dims[1]  %08x\n",     pri[i].dims[1]);
        enlight_detector_dbg("\n");
        enlight_detector_dbg(" loc.buf      %08x\n",     loc[i].buf );
        enlight_detector_dbg(" loc.scl      %08x\n",     loc[i].scl);
        enlight_detector_dbg(" loc.dtype    %08x\n",     loc[i].dtype);
        enlight_detector_dbg(" loc.num_ele  %08x\n",     loc[i].num_ele);
        enlight_detector_dbg(" loc.num_dim  %08x\n",     loc[i].num_dim);
        enlight_detector_dbg(" loc.dims[0]  %08x\n",     loc[i].dims[0]);
        enlight_detector_dbg(" loc.dims[1]  %08x\n",     loc[i].dims[1]);
        enlight_detector_dbg(" loc.dims[2]  %08x\n",     loc[i].dims[2]);
        enlight_detector_dbg("\n");
        enlight_detector_dbg(" scr.buf      %08x\n",     scr[i].buf);
        enlight_detector_dbg(" scr.scl      %08x\n",     scr[i].scl);
        enlight_detector_dbg(" scr.dtype    %08x\n",     scr[i].dtype);
        enlight_detector_dbg(" scr.num_ele  %08x\n",     scr[i].num_ele);
        enlight_detector_dbg(" scr.num_dim  %08x\n",     scr[i].num_dim);
        enlight_detector_dbg(" scr.dims[0]  %08x\n",     scr[i].dims[0]);
        enlight_detector_dbg(" scr.dims[1]  %08x\n",     scr[i].dims[1]);
        enlight_detector_dbg(" scr.dims[2]  %08x\n",     scr[i].dims[2]);
    }

#ifdef POST_PROCESS_REPORT_CYCLE
#ifdef __i386__
    asm volatile ("rdtsc" : "=a"(cycle0), "=d"(cycle0_h));
#else
    asm volatile ("rdcycle %0" : "=r" (cycle0));
#endif
#endif //POST_PROCESS_REPORT_CYCLE

    detection_t* r1 = yolo_post_process(scr, loc, pri, obj,
        th_conf, th_iou, num_grids, num_output, softmax_use);

#ifdef POST_PROCESS_REPORT_CYCLE
#ifdef __i386__
    asm volatile ("rdtsc" : "=a"(cycle1), "=d"(cycle1_h));
#else
    asm volatile ("rdcycle %0" : "=r" (cycle1));
#endif
    enlight_detector_dbg("[Postproc cycles] %ld \n", cycle1 - cycle0);
#endif //POST_PROCESS_REPORT_CYCLE

    enlight_detector_dbg("%d object is detected\n", r1->n);

    for (i = 0 ; i < r1->n ; i++) {
        box_t* b = r1->box + i;

        enlight_detector_dbg("x_min: %d, x_max: %d, y_min: %d, y_max: %d, score: %d, class: %d\n",
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
        dbg_obj->class = r1->class[i] + 1; // + 1 for background
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
        ret_obj->class = r1->class[i] + 1; // + 1 for background
        ret_obj->img_w = img_sizes[1];
        ret_obj->img_h = img_sizes[0];
        detected_objs->cnt = r1->n;
    }

    enlight_free(r1);

    for(i = 0 ; i < num_output; i++)
        enlight_free(output_work_buf_base[i]);

    enlight_detector_dbg_fout();

    return 0;
}
