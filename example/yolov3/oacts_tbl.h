
#ifndef __OACTS_TBL_H__
#define __OACTS_TBL_H__

/* How to get i'th value
if (oact->log2_scale >= 0)
    val = (float)oact->buf[i] / (1 << oact->log2_scale)
else
    val = (float)oact->buf[i] * (1 << (-oact->log2_scale))
*/

act_tensor_t Conv2d_58 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x51380, .size = 12675, .fb = 4,
    .num_dimension = 4, .dimensions = {1,75,13,13},
    .log2_scale = 1, .zp = 0
};

act_tensor_t Conv2d_66 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x0, .size = 50700, .fb = 4,
    .num_dimension = 4, .dimensions = {1,75,26,26},
    .log2_scale = 2, .zp = 0
};

act_tensor_t Conv2d_73_swap = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x10d80, .size = 202800, .fb = 4,
    .num_dimension = 4, .dimensions = {1,75,52,52},
    .log2_scale = 4, .zp = 0
};
/*

oact buffer layout
                            layer       base       size fb
(                     Conv2d_58,    0x51380,     0x3183,  4),
(                     Conv2d_66,        0x0,     0xc60c,  4),
(                Conv2d_73.swap,    0x10d80,    0x31830,  4),
*/
#endif /*__OACTS_TBL_H__*/
