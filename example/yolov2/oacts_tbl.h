
#ifndef __OACTS_TBL_H__
#define __OACTS_TBL_H__

/* How to get i'th value
if (oact->log2_scale >= 0)
    val = (float)oact->buf[i] / (1 << oact->log2_scale)
else
    val = (float)oact->buf[i] * (1 << (-oact->log2_scale))
*/

act_tensor_t Conv2d_22 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x0, .size = 21125, .fb = 4,
    .num_dimension = 4, .dimensions = {1,125,13,13},
    .log2_scale = 2, .zp = 0
};
/*

oact buffer layout
                            layer       base       size fb
(                     Conv2d_22,        0x0,     0x5285,  4),
*/
#endif /*__OACTS_TBL_H__*/
