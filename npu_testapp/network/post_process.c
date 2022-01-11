
/*
    This is generated at 2020-11-23 16:08:04.712054
    Openedges Enlight post process main
*/

#include "stdint.h"
#include "npu_data_type.h"
#include "oacts_tbl.h"
#include "quantized_prior_box.h"

void detector_init(void *base);

void detector_run(
    uint8_t *prior_box,
    uint8_t *loc_buf_32,
    uint8_t *score_buf_32,
    int log2_prior_scl,
    int log2_loc_scl,
    int log2_score_scl,
    int num_class, int num_stage, int* grids,
    int* vars, int th_conf, int th_iou, int num_box);

void run_post_process(void *oact_base, void *work_base)
{

    uint8_t* oact_base_addr;
    uint8_t* loc_buf_addr;
    uint8_t* score_buf_addr;
    uint8_t* prior_box_addr;

    int log2_prior_scl;
    int log2_score_scl;
    int log2_loc_scl;
    int num_class;
    int num_stage;
    int *grids;
    int *vars;
    int th_conf, th_iou;
    int num_box;

    act_tensor_t *oact_score = &Concat_269;
    act_tensor_t *oact_loc = &Concat_270;

    oact_base_addr = (uint8_t*)oact_base;
    loc_buf_addr = oact_base_addr + (unsigned long)(oact_loc->buf);
    score_buf_addr = oact_base_addr + (unsigned long)(oact_score->buf);
    prior_box_addr = prior_box;

    log2_prior_scl = log2_prior_box_scale;
    log2_score_scl = oact_score->log2_scale;
    log2_loc_scl = oact_loc->log2_scale;
    num_class = detector_num_class + 1;
    num_stage = detector_stages;
    grids = detector_grids;
    vars = detector_var;
    th_conf = detector_th_conf;
    th_iou = detector_th_iou;
    num_box = detector_num_anchor;

    detector_init(work_base);

    detector_run(prior_box_addr, loc_buf_addr, score_buf_addr,
        log2_prior_scl, log2_loc_scl, log2_score_scl,
        num_class, num_stage, grids,
        vars, th_conf, th_iou, num_box);

}