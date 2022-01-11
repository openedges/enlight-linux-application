
#ifndef __OACTS_TBL_H__
#define __OACTS_TBL_H__

/* How to get i'th value
if (oact->log2_scale >= 0)
    val = (float)oact->buf[i] / (1 << oact->log2_scale)
else
    val = (float)oact->buf[i] * (1 << (-oact->log2_scale))
*/

act_tensor_t Concat_269 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x18700, .size = 96000, .fb = 4,
    .num_dimension = 4, .dimensions = {1,32,1,3000},
    .log2_scale = 2, .zp = 0
};

act_tensor_t Concat_270 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x0, .size = 96000, .fb = 4,
    .num_dimension = 4, .dimensions = {1,32,1,3000},
    .log2_scale = 5, .zp = 0
};
/*

oact buffer layout
                            layer       base       size fb
(                    Concat_269,    0x18700,    0x17700,  4),
(                    Concat_270,        0x0,    0x17700,  4),
*/
#endif /*__OACTS_TBL_H__*/
