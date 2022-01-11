
#ifndef __OACTS_TBL_H__
#define __OACTS_TBL_H__

/* How to get i'th value
if (oact->log2_scale >= 0)
    val = (float)oact->buf[i] / (1 << oact->log2_scale)
else
    val = (float)oact->buf[i] * (1 << (-oact->log2_scale))
*/

act_tensor_t Concat_878 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x180a00, .size = 1571328, .fb = 4,
    .num_dimension = 4, .dimensions = {1,32,1,49104},
    .log2_scale = 1, .zp = 0
};

act_tensor_t Concat_879 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x0, .size = 1571328, .fb = 4,
    .num_dimension = 4, .dimensions = {1,32,1,49104},
    .log2_scale = 7, .zp = 0
};
/*

oact buffer layout
                            layer       base       size fb
(                    Concat_878,   0x180a00,   0x17fa00,  4),
(                    Concat_879,        0x0,   0x17fa00,  4),
*/
#endif /*__OACTS_TBL_H__*/
