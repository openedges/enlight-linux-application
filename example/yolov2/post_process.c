
/*
    Openedges Enlight post process main
*/

#include "stdint.h"
#include "npu_data_type.h"
#include "oacts_tbl.h"
#include "quantized_prior_box.h"

extern void yolo_init_detector(void *base);

extern void yolo_run_detector(
    void** output, void** prior_box,
    uint32_t** out_exp_tbl, uint32_t** out_sig_tbl,
    int* output_scales, int pri_scl, int softmax_use,
    int num_class, int num_anchor, int* num_grids, int num_output,
    int img_h, int img_w,
    int th_conf, int th_iou);

uint32_t yolo_out_exp_table[1][256] = {
    {
     //score_scl 2
     0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
     0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
     0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
     0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
     0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
     0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
     0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000002, 0x00000002, 0x00000003, 0x00000004, 
     0x00000005, 0x00000006, 0x00000008, 0x0000000a, 0x0000000d, 0x00000010, 0x00000015, 0x0000001b, 0x00000023, 0x0000002c, 0x00000039, 0x00000049, 0x0000005e, 0x00000079, 0x0000009b, 0x000000c7, 
     0x00000100, 0x00000149, 0x000001a6, 0x0000021e, 0x000002b8, 0x0000037e, 0x0000047b, 0x000005c1, 0x00000764, 0x0000097d, 0x00000c2f, 0x00000fa5, 0x00001416, 0x000019ca, 0x0000211e, 0x00002a85, 
     0x00003699, 0x0000461b, 0x00005a04, 0x00007396, 0x0000946a, 0x0000be91, 0x0000f4b1, 0x00013a31, 0x0001936e, 0x00020603, 0x00029924, 0x0003560f, 0x000448a2, 0x0005801b, 0x0007100b, 0x00091193, 
     0x000ba4f5, 0x000ef3a0, 0x001332c5, 0x0018a6b0, 0x001fa715, 0x0028a491, 0x00342fba, 0x0043023b, 0x00560a77, 0x006e7a8b, 0x008ddb81, 0x00b62607, 0x00e9e224, 0x012c4feb, 0x01819bc5, 0x01ef218f, 
     0x027bc2cb, 0x0330554a, 0x04183149, 0x0541e7e5, 0x06c02d64, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 
     0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 
     0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 
     0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 
     0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 0x07ffffff, 
    },

};

uint32_t yolo_out_sig_table[1][256] = {
    {
     //score_scl 2
     0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
     0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
     0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
     0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
     0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
     0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
     0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000002, 0x00000002, 0x00000003, 0x00000004, 
     0x00000005, 0x00000006, 0x00000008, 0x0000000a, 0x0000000c, 0x0000000f, 0x00000013, 0x00000018, 0x0000001f, 0x00000026, 0x0000002f, 0x00000039, 0x00000045, 0x00000052, 0x00000061, 0x00000070, 
     0x00000080, 0x00000090, 0x0000009f, 0x000000ae, 0x000000bb, 0x000000c7, 0x000000d1, 0x000000da, 0x000000e1, 0x000000e8, 0x000000ed, 0x000000f1, 0x000000f4, 0x000000f6, 0x000000f8, 0x000000fa, 
     0x000000fb, 0x000000fc, 0x000000fd, 0x000000fe, 0x000000fe, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 
     0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 
     0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 
     0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 
     0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 
     0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 
     0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, 
    },

};

int yolo_num_grids[1*2]= {
    13, 13,
};
int yolo_output_scales[1] = {
    2, 
};
void run_post_process(void *oact_base, void *work_base)
{

    act_tensor_t   *yolo_output_0 = &Conv2d_22;

    int softmax_use    = 1;
    int pri_scl        = 8;
    int num_anchor     = 5;
    int num_class      = 20;
    int num_output     = 1;
    int th_conf        = 128;
    int th_iou         = 128;
    int img_h          = 416;
    int img_w          = 416;

    void*     prior_boxes[1] = {
         yolo_prior_box + 4 * 5 * 0,
    };
    void*     outputs[1] = {
        (uint8_t*)oact_base + (unsigned long)(yolo_output_0->buf),
    };
    uint32_t* out_exp_tbls[1] = {
        &yolo_out_exp_table[0][0],
    };
    uint32_t*  out_sig_tbls[1] = {
        &yolo_out_sig_table[0][0],
    };


    yolo_init_detector(work_base);

    yolo_run_detector(
         outputs, prior_boxes,
         out_exp_tbls, out_sig_tbls,
         yolo_output_scales, pri_scl, softmax_use,
         num_class, num_anchor, yolo_num_grids, num_output,
         img_h, img_w,
         th_conf, th_iou);
}

#ifdef TEST_ON_PC
void get_yolo_network_info(
    int *output_scales, int *pri_scl,
    int *softmax_use,
    int *num_output,
    int *num_anchor, int *num_class, int** num_grids,
    int *th_conf, int *th_iou, int *img_h, int *img_w,
    uint8_t** prior_box_buf, uint32_t (**out_exp_tbl)[256], uint32_t (**out_sig_tbl)[256])
{
    output_scales[0] = 2.;

    *softmax_use = 1;
    *pri_scl     = 8;
    *num_output  = 1;
    *num_anchor  = 5;
    *num_class   = 20;
    *num_grids   = yolo_num_grids;
    *th_conf     = 128;
    *th_iou      = 128;
    *img_h       = 416;
    *img_w       = 416;

    *prior_box_buf = yolo_prior_box;
    *out_exp_tbl   = yolo_out_exp_table;
    *out_sig_tbl   = yolo_out_sig_table;
}
#endif