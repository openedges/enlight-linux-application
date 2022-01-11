
/*
    Openedges Enlight post process main
*/

#include "stdint.h"
#include "npu_data_type.h"
#include "oacts_tbl.h"
#include "quantized_prior_box.h"

uint32_t ssd_scr_exp_table[256] = {
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
};

uint32_t ssd_loc_exp_table[256] = {
     //loc_scl 5
     0x00000005, 0x00000005, 0x00000005, 0x00000005, 0x00000005, 0x00000005, 0x00000006, 0x00000006, 0x00000006, 0x00000006, 0x00000006, 0x00000007, 0x00000007, 0x00000007, 0x00000007, 0x00000007, 
     0x00000008, 0x00000008, 0x00000008, 0x00000008, 0x00000009, 0x00000009, 0x00000009, 0x0000000a, 0x0000000a, 0x0000000a, 0x0000000b, 0x0000000b, 0x0000000b, 0x0000000c, 0x0000000c, 0x0000000c, 
     0x0000000d, 0x0000000d, 0x0000000e, 0x0000000e, 0x0000000e, 0x0000000f, 0x0000000f, 0x00000010, 0x00000010, 0x00000011, 0x00000011, 0x00000012, 0x00000013, 0x00000013, 0x00000014, 0x00000014, 
     0x00000015, 0x00000016, 0x00000016, 0x00000017, 0x00000018, 0x00000019, 0x00000019, 0x0000001a, 0x0000001b, 0x0000001c, 0x0000001d, 0x0000001e, 0x0000001f, 0x00000020, 0x00000021, 0x00000022, 
     0x00000023, 0x00000024, 0x00000025, 0x00000026, 0x00000027, 0x00000029, 0x0000002a, 0x0000002b, 0x0000002c, 0x0000002e, 0x0000002f, 0x00000031, 0x00000032, 0x00000034, 0x00000036, 0x00000037, 
     0x00000039, 0x0000003b, 0x0000003d, 0x0000003f, 0x00000041, 0x00000043, 0x00000045, 0x00000047, 0x00000049, 0x0000004c, 0x0000004e, 0x00000051, 0x00000053, 0x00000056, 0x00000058, 0x0000005b, 
     0x0000005e, 0x00000061, 0x00000064, 0x00000067, 0x0000006b, 0x0000006e, 0x00000072, 0x00000075, 0x00000079, 0x0000007d, 0x00000081, 0x00000085, 0x00000089, 0x0000008d, 0x00000092, 0x00000096, 
     0x0000009b, 0x000000a0, 0x000000a5, 0x000000ab, 0x000000b0, 0x000000b6, 0x000000bb, 0x000000c1, 0x000000c7, 0x000000ce, 0x000000d4, 0x000000db, 0x000000e2, 0x000000e9, 0x000000f0, 0x000000f8, 
     0x00000100, 0x00000108, 0x00000111, 0x00000119, 0x00000122, 0x0000012b, 0x00000135, 0x0000013f, 0x00000149, 0x00000153, 0x0000015e, 0x00000169, 0x00000174, 0x00000180, 0x0000018d, 0x00000199, 
     0x000001a6, 0x000001b3, 0x000001c1, 0x000001d0, 0x000001de, 0x000001ed, 0x000001fd, 0x0000020d, 0x0000021e, 0x0000022f, 0x00000241, 0x00000253, 0x00000266, 0x0000027a, 0x0000028e, 0x000002a2, 
     0x000002b8, 0x000002ce, 0x000002e5, 0x000002fc, 0x00000315, 0x0000032e, 0x00000347, 0x00000362, 0x0000037e, 0x0000039a, 0x000003b7, 0x000003d5, 0x000003f4, 0x00000415, 0x00000436, 0x00000458, 
     0x0000047b, 0x000004a0, 0x000004c5, 0x000004ec, 0x00000514, 0x0000053d, 0x00000568, 0x00000594, 0x000005c1, 0x000005f0, 0x00000620, 0x00000652, 0x00000685, 0x000006ba, 0x000006f1, 0x00000729, 
     0x00000764, 0x000007a0, 0x000007de, 0x0000081e, 0x0000085f, 0x000008a4, 0x000008ea, 0x00000932, 0x0000097d, 0x000009ca, 0x00000a1a, 0x00000a6c, 0x00000ac0, 0x00000b18, 0x00000b72, 0x00000bcf, 
     0x00000c2f, 0x00000c92, 0x00000cf8, 0x00000d61, 0x00000dce, 0x00000e3e, 0x00000eb2, 0x00000f29, 0x00000fa5, 0x00001024, 0x000010a7, 0x0000112e, 0x000011ba, 0x0000124a, 0x000012de, 0x00001378, 
     0x00001416, 0x000014b9, 0x00001562, 0x0000160f, 0x000016c3, 0x0000177b, 0x0000183a, 0x000018ff, 0x000019ca, 0x00001a9c, 0x00001b74, 0x00001c53, 0x00001d39, 0x00001e27, 0x00001f1c, 0x00002019, 
     0x0000211e, 0x0000222b, 0x00002340, 0x0000245f, 0x00002586, 0x000026b7, 0x000027f2, 0x00002936, 0x00002a85, 0x00002bdf, 0x00002d43, 0x00002eb3, 0x0000302f, 0x000031b6, 0x0000334a, 0x000034eb, 
};

void ssd_detector_init(void *base);

void ssd_detector_run(
    uint8_t *prior_box,
    uint8_t *loc_buf_32,
    uint8_t *score_buf_32,
    uint32_t *loc_exp_tbl, uint32_t *score_exp_tbl,
    int prior_scl, int loc_scl, int score_scl,
    int num_stage, int* grids,
    int num_class, int num_box, int img_size,
    int th_conf, int th_iou);

void run_post_process(void *oact_base, void *work_base)
{

    uint8_t* oact_base_addr;
    uint8_t* loc_buf_addr;
    uint8_t* score_buf_addr;
    uint8_t* prior_box_addr;

    int prior_scl, score_scl, loc_scl;
    int num_class;
    int num_stage;
    int *grids;
    int th_conf, th_iou;
    int num_box, img_size;

    act_tensor_t *oact_score = &Concat_269;
    act_tensor_t *oact_loc = &Concat_270;

    uint32_t* loc_exp_tbl = ssd_loc_exp_table;
    uint32_t* score_exp_tbl = ssd_scr_exp_table;

    oact_base_addr = (uint8_t*)oact_base;
    loc_buf_addr = oact_base_addr + (unsigned long)(oact_loc->buf);
    score_buf_addr = oact_base_addr + (unsigned long)(oact_score->buf);
    prior_box_addr = prior_box;

    prior_scl = ssd_prior_box_scale;
    score_scl = oact_score->log2_scale;
    loc_scl   = oact_loc->log2_scale;
    num_class = ssd_detector_num_class + 1;
    num_stage = ssd_detector_stages;
    grids     = ssd_detector_grids;
    th_conf   = ssd_detector_th_conf;
    th_iou    = ssd_detector_th_iou;
    num_box   = ssd_detector_num_anchor;
    img_size  = ssd_detector_img_size;

    ssd_detector_init(work_base);

    ssd_detector_run(prior_box_addr, loc_buf_addr, score_buf_addr,
        loc_exp_tbl, score_exp_tbl,
        prior_scl, loc_scl, score_scl,
        num_stage, grids, num_class, num_box, img_size,
        th_conf, th_iou);
}
#ifdef TEST_ON_PC
void get_ssd_network_info(
    uint8_t** prior_box_addr,
    uint32_t** loc_exp_tbl, uint32_t** score_exp_tbl,
    int *prior_scl, int *loc_scl, int *score_scl,
    int *num_stage, int **grids, int *num_class, int *num_box, int *img_size,
    int *th_conf, int *th_iou)
{
    *prior_box_addr = prior_box;
    *loc_exp_tbl    = ssd_loc_exp_table;
    *score_exp_tbl  = ssd_scr_exp_table;
    *prior_scl      = ssd_prior_box_scale;
    *score_scl      = Concat_269.log2_scale;
    *loc_scl        = Concat_270.log2_scale;
    *num_class      = ssd_detector_num_class + 1;
    *num_stage      = ssd_detector_stages;
    *grids          = ssd_detector_grids;
    *th_conf        = ssd_detector_th_conf;
    *th_iou         = ssd_detector_th_iou;
    *num_box        = ssd_detector_num_anchor;
    *img_size       = ssd_detector_img_size;
}
#endif