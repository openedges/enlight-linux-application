
#ifndef __QUANTIZED_NETWORK_OACT_H__
#define __QUANTIZED_NETWORK_OACT_H__
#include "npu_data_type.h"


act_tensor_t oact_input1 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x0, .size = 274432, .fb = 3,
    .num_dimension = 4, .dimensions = {1, 3, 300, 300},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Relu_0 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x0, .size = 720896, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 150, 150},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Relu_1 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xb0000, .size = 720896, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 150, 150},
    .log2_scale = 3, .zp = 0
};

act_tensor_t oact_BatchNormalization_2 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x160000, .size = 720896, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 16, 150, 150},
    .log2_scale = 3, .zp = 0
};

act_tensor_t oact_Relu_2 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x210000, .size = 2162688, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 96, 150, 150},
    .log2_scale = 3, .zp = 0
};

act_tensor_t oact_Relu_3 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x420000, .size = 540672, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 96, 75, 75},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_BatchNormalization_5 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x4a4000, .size = 180224, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 24, 75, 75},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Relu_4 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x4d0000, .size = 901120, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 144, 75, 75},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_5 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x5ac000, .size = 901120, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 144, 75, 75},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_BatchNormalization_8 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x688000, .size = 180224, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 24, 75, 75},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Add_0 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x6b4000, .size = 180224, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 24, 75, 75},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Relu_6 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x6e0000, .size = 901120, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 144, 75, 75},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_7 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x7bc000, .size = 233472, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 144, 38, 38},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_BatchNormalization_11 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x7f5000, .size = 49152, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 38, 38},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_8 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x801000, .size = 278528, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 38, 38},
    .log2_scale = 6, .zp = 0
};

act_tensor_t oact_Relu_9 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x845000, .size = 278528, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 38, 38},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_BatchNormalization_14 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x889000, .size = 49152, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 38, 38},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Add_1 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x895000, .size = 49152, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 38, 38},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Relu_10 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x8a1000, .size = 278528, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 38, 38},
    .log2_scale = 6, .zp = 0
};

act_tensor_t oact_Relu_11 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x8e5000, .size = 278528, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 38, 38},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_BatchNormalization_17 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x929000, .size = 49152, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 38, 38},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Add_2 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x935000, .size = 49152, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 38, 38},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Relu_12 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x941000, .size = 278528, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 38, 38},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_13 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x985000, .size = 69632, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 19, 19},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_BatchNormalization_20 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x996000, .size = 24576, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 64, 19, 19},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Relu_14 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x99c000, .size = 139264, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 384, 19, 19},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_15 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x9be000, .size = 139264, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 384, 19, 19},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_BatchNormalization_23 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x9e0000, .size = 24576, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 64, 19, 19},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Add_3 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x9e6000, .size = 24576, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 64, 19, 19},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Relu_16 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x9ec000, .size = 139264, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 384, 19, 19},
    .log2_scale = 6, .zp = 0
};

act_tensor_t oact_Relu_17 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xa0e000, .size = 139264, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 384, 19, 19},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_BatchNormalization_26 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xa30000, .size = 24576, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 64, 19, 19},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Add_4 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xa36000, .size = 24576, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 64, 19, 19},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Relu_18 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xa3c000, .size = 139264, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 384, 19, 19},
    .log2_scale = 6, .zp = 0
};

act_tensor_t oact_Relu_19 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xa5e000, .size = 139264, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 384, 19, 19},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_BatchNormalization_29 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xa80000, .size = 24576, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 64, 19, 19},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Add_5 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xa86000, .size = 24576, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 64, 19, 19},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Relu_20 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xa8c000, .size = 139264, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 384, 19, 19},
    .log2_scale = 6, .zp = 0
};

act_tensor_t oact_Relu_21 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xaae000, .size = 139264, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 384, 19, 19},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_BatchNormalization_32 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xad0000, .size = 36864, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 96, 19, 19},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_22 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xad9000, .size = 208896, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 576, 19, 19},
    .log2_scale = 6, .zp = 0
};

act_tensor_t oact_Relu_23 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xb0c000, .size = 208896, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 576, 19, 19},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_BatchNormalization_35 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xb3f000, .size = 36864, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 96, 19, 19},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Add_6 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xb48000, .size = 36864, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 96, 19, 19},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_24 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xb51000, .size = 208896, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 576, 19, 19},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_25 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xb84000, .size = 208896, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 576, 19, 19},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_BatchNormalization_38 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xbb7000, .size = 36864, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 96, 19, 19},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Add_7 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xbc0000, .size = 36864, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 96, 19, 19},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Relu_26 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xbc9000, .size = 208896, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 576, 19, 19},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_44 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xbfc000, .size = 208896, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 576, 19, 19},
    .log2_scale = 1, .zp = 0
};

act_tensor_t oact_Conv_67 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xc2f000, .size = 69632, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 19, 19},
    .log2_scale = 1, .zp = 0
};

act_tensor_t oact_Transpose_1 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xc2f000, .size = 69632, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 19, 19, 192},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_real_Reshape_6 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xc2f000, .size = 69632, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 2166, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Reshape_6 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xc2f000, .size = 69632, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 2166, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Relu_43 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xc40000, .size = 208896, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 576, 19, 19},
    .log2_scale = 2, .zp = 0
};

act_tensor_t oact_Conv_65 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xc73000, .size = 69632, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 19, 19},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Transpose_0 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xc73000, .size = 69632, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 19, 19, 192},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_real_Reshape_0 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xc73000, .size = 69632, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 2166, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Reshape_0 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xc73000, .size = 69632, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 2166, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Relu_27 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xc84000, .size = 61440, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 576, 10, 10},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_BatchNormalization_41 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xc93000, .size = 16384, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 160, 10, 10},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_28 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xc97000, .size = 98304, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 960, 10, 10},
    .log2_scale = 6, .zp = 0
};

act_tensor_t oact_Relu_29 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xcaf000, .size = 98304, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 960, 10, 10},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_BatchNormalization_44 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xcc7000, .size = 16384, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 160, 10, 10},
    .log2_scale = 6, .zp = 0
};

act_tensor_t oact_Add_8 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xccb000, .size = 16384, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 160, 10, 10},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_30 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xccf000, .size = 98304, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 960, 10, 10},
    .log2_scale = 6, .zp = 0
};

act_tensor_t oact_Relu_31 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xce7000, .size = 98304, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 960, 10, 10},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_BatchNormalization_47 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xcff000, .size = 16384, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 160, 10, 10},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Add_9 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xd03000, .size = 16384, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 160, 10, 10},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_32 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xd07000, .size = 98304, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 960, 10, 10},
    .log2_scale = 6, .zp = 0
};

act_tensor_t oact_Relu_33 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xd1f000, .size = 98304, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 960, 10, 10},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_BatchNormalization_50 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xd37000, .size = 32768, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 320, 10, 10},
    .log2_scale = 6, .zp = 0
};

act_tensor_t oact_Relu_34 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xd3f000, .size = 131072, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 1280, 10, 10},
    .log2_scale = 3, .zp = 0
};

act_tensor_t oact_Relu_46 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xd5f000, .size = 131072, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 1280, 10, 10},
    .log2_scale = 2, .zp = 0
};

act_tensor_t oact_Conv_71 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xd7f000, .size = 20480, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 10, 10},
    .log2_scale = 1, .zp = 0
};

act_tensor_t oact_Transpose_3 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xd7f000, .size = 20480, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 10, 10, 192},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_real_Reshape_7 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xd7f000, .size = 20480, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 600, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Reshape_7 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xd7f000, .size = 20480, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 600, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Relu_45 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xd84000, .size = 131072, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 1280, 10, 10},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Conv_69 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xda4000, .size = 20480, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 10, 10},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Transpose_2 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xda4000, .size = 20480, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 10, 10, 192},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_real_Reshape_1 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xda4000, .size = 20480, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 600, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Reshape_1 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xda4000, .size = 20480, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 600, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Relu_35 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xda9000, .size = 28672, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 256, 10, 10},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_36 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdb0000, .size = 8192, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 256, 5, 5},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_BatchNormalization_54 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdb2000, .size = 16384, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 512, 5, 5},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Relu_48 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdb6000, .size = 16384, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 512, 5, 5},
    .log2_scale = 3, .zp = 0
};

act_tensor_t oact_Conv_75 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdba000, .size = 8192, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 5, 5},
    .log2_scale = 1, .zp = 0
};

act_tensor_t oact_Transpose_5 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdba000, .size = 8192, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 5, 5, 192},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_real_Reshape_8 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdba000, .size = 8192, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 150, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Reshape_8 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdba000, .size = 8192, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 150, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Relu_47 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdbc000, .size = 16384, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 512, 5, 5},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Conv_73 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc0000, .size = 8192, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 5, 5},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Transpose_4 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc0000, .size = 8192, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 5, 5, 192},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_real_Reshape_2 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc0000, .size = 8192, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 150, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Reshape_2 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc0000, .size = 8192, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 150, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Relu_37 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc2000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 128, 5, 5},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_38 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc3000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 128, 3, 3},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_BatchNormalization_57 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc4000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 256, 3, 3},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_50 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc5000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 256, 3, 3},
    .log2_scale = 2, .zp = 0
};

act_tensor_t oact_Conv_79 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc6000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 3, 3},
    .log2_scale = 1, .zp = 0
};

act_tensor_t oact_Transpose_7 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc6000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 3, 3, 192},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_real_Reshape_9 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc6000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 54, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Reshape_9 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc6000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 54, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Relu_49 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc7000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 256, 3, 3},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Conv_77 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc8000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 3, 3},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Transpose_6 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc8000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 3, 3, 192},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_real_Reshape_3 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc8000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 54, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Reshape_3 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc8000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 54, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Relu_39 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdc9000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 128, 3, 3},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_40 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdca000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 128, 2, 2},
    .log2_scale = 6, .zp = 0
};

act_tensor_t oact_BatchNormalization_60 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdcb000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 256, 2, 2},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Relu_52 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdcc000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 256, 2, 2},
    .log2_scale = 3, .zp = 0
};

act_tensor_t oact_Conv_83 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdcd000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 2, 2},
    .log2_scale = 1, .zp = 0
};

act_tensor_t oact_Transpose_9 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdcd000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 2, 2, 192},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_real_Reshape_10 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdcd000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 24, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Reshape_10 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdcd000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 24, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Relu_51 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdce000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 256, 2, 2},
    .log2_scale = 4, .zp = 0
};

act_tensor_t oact_Conv_81 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdcf000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 2, 2},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Transpose_8 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdcf000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 2, 2, 192},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_real_Reshape_4 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdcf000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 24, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Reshape_4 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdcf000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 24, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Relu_41 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdd0000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 64, 2, 2},
    .log2_scale = 6, .zp = 0
};

act_tensor_t oact_Relu_42 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdd1000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 64, 1, 1},
    .log2_scale = 6, .zp = 0
};

act_tensor_t oact_BatchNormalization_63 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdd2000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 64, 1, 1},
    .log2_scale = 3, .zp = 0
};

act_tensor_t oact_Conv_85 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdd3000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 1, 1},
    .log2_scale = 1, .zp = 0
};

act_tensor_t oact_Transpose_11 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdd3000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 1, 1, 192},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_real_Reshape_11 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdd3000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 6, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Reshape_11 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdd3000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 6, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Concat_1 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x0, .size = 98304, .fb = 4,
    .num_dimension = 3, .dimensions = {1, 3000, 21},
    .log2_scale = 1, .zp = 0
};

act_tensor_t oact_Softmax_0 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x0, .size = 98304, .fb = 4,
    .num_dimension = 3, .dimensions = {1, 3000, 21},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_740 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x18000, .size = 0, .fb = 4,
    .num_dimension = 3, .dimensions = {1, 3000, 21},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Conv_84 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdd4000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 192, 1, 1},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_Transpose_10 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdd4000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 1, 1, 192},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_real_Reshape_5 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdd4000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 6, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Reshape_5 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0xdd4000, .size = 4096, .fb = 2,
    .num_dimension = 4, .dimensions = {1, 32, 6, 1},
    .log2_scale = -1, .zp = 0
};

act_tensor_t oact_Concat_0 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x18000, .size = 98304, .fb = 4,
    .num_dimension = 3, .dimensions = {1, 3000, 4},
    .log2_scale = 5, .zp = 0
};

act_tensor_t oact_726 = {
    .data_type = TYPE_INT8,
    .buf = (void*)0x30000, .size = 0, .fb = 4,
    .num_dimension = 3, .dimensions = {1, 3000, 4},
    .log2_scale = -1, .zp = 0
};
/*

oact buffer layout
                            layer       base       size fb
(                     'input.1',        0x0,    0x43000,  3),
(                      'Relu_0',        0x0,    0xb0000,  2),
(                      'Relu_1',    0xb0000,    0xb0000,  2),
(        'BatchNormalization_2',   0x160000,    0xb0000,  2),
(                      'Relu_2',   0x210000,   0x210000,  2),
(                      'Relu_3',   0x420000,    0x84000,  2),
(        'BatchNormalization_5',   0x4a4000,    0x2c000,  2),
(                      'Relu_4',   0x4d0000,    0xdc000,  2),
(                      'Relu_5',   0x5ac000,    0xdc000,  2),
(        'BatchNormalization_8',   0x688000,    0x2c000,  2),
(                       'Add_0',   0x6b4000,    0x2c000,  2),
(                      'Relu_6',   0x6e0000,    0xdc000,  2),
(                      'Relu_7',   0x7bc000,    0x39000,  2),
(       'BatchNormalization_11',   0x7f5000,     0xc000,  2),
(                      'Relu_8',   0x801000,    0x44000,  2),
(                      'Relu_9',   0x845000,    0x44000,  2),
(       'BatchNormalization_14',   0x889000,     0xc000,  2),
(                       'Add_1',   0x895000,     0xc000,  2),
(                     'Relu_10',   0x8a1000,    0x44000,  2),
(                     'Relu_11',   0x8e5000,    0x44000,  2),
(       'BatchNormalization_17',   0x929000,     0xc000,  2),
(                       'Add_2',   0x935000,     0xc000,  2),
(                     'Relu_12',   0x941000,    0x44000,  2),
(                     'Relu_13',   0x985000,    0x11000,  2),
(       'BatchNormalization_20',   0x996000,     0x6000,  2),
(                     'Relu_14',   0x99c000,    0x22000,  2),
(                     'Relu_15',   0x9be000,    0x22000,  2),
(       'BatchNormalization_23',   0x9e0000,     0x6000,  2),
(                       'Add_3',   0x9e6000,     0x6000,  2),
(                     'Relu_16',   0x9ec000,    0x22000,  2),
(                     'Relu_17',   0xa0e000,    0x22000,  2),
(       'BatchNormalization_26',   0xa30000,     0x6000,  2),
(                       'Add_4',   0xa36000,     0x6000,  2),
(                     'Relu_18',   0xa3c000,    0x22000,  2),
(                     'Relu_19',   0xa5e000,    0x22000,  2),
(       'BatchNormalization_29',   0xa80000,     0x6000,  2),
(                       'Add_5',   0xa86000,     0x6000,  2),
(                     'Relu_20',   0xa8c000,    0x22000,  2),
(                     'Relu_21',   0xaae000,    0x22000,  2),
(       'BatchNormalization_32',   0xad0000,     0x9000,  2),
(                     'Relu_22',   0xad9000,    0x33000,  2),
(                     'Relu_23',   0xb0c000,    0x33000,  2),
(       'BatchNormalization_35',   0xb3f000,     0x9000,  2),
(                       'Add_6',   0xb48000,     0x9000,  2),
(                     'Relu_24',   0xb51000,    0x33000,  2),
(                     'Relu_25',   0xb84000,    0x33000,  2),
(       'BatchNormalization_38',   0xbb7000,     0x9000,  2),
(                       'Add_7',   0xbc0000,     0x9000,  2),
(                     'Relu_26',   0xbc9000,    0x33000,  2),
(                     'Relu_44',   0xbfc000,    0x33000,  2),
(                     'Conv_67',   0xc2f000,    0x11000,  2),
(                 'Transpose_1',   0xc2f000,    0x11000,  2),
(              'real_Reshape_6',   0xc2f000,    0x11000,  2),
(                   'Reshape_6',   0xc2f000,    0x11000,  2),
(                     'Relu_43',   0xc40000,    0x33000,  2),
(                     'Conv_65',   0xc73000,    0x11000,  2),
(                 'Transpose_0',   0xc73000,    0x11000,  2),
(              'real_Reshape_0',   0xc73000,    0x11000,  2),
(                   'Reshape_0',   0xc73000,    0x11000,  2),
(                     'Relu_27',   0xc84000,     0xf000,  2),
(       'BatchNormalization_41',   0xc93000,     0x4000,  2),
(                     'Relu_28',   0xc97000,    0x18000,  2),
(                     'Relu_29',   0xcaf000,    0x18000,  2),
(       'BatchNormalization_44',   0xcc7000,     0x4000,  2),
(                       'Add_8',   0xccb000,     0x4000,  2),
(                     'Relu_30',   0xccf000,    0x18000,  2),
(                     'Relu_31',   0xce7000,    0x18000,  2),
(       'BatchNormalization_47',   0xcff000,     0x4000,  2),
(                       'Add_9',   0xd03000,     0x4000,  2),
(                     'Relu_32',   0xd07000,    0x18000,  2),
(                     'Relu_33',   0xd1f000,    0x18000,  2),
(       'BatchNormalization_50',   0xd37000,     0x8000,  2),
(                     'Relu_34',   0xd3f000,    0x20000,  2),
(                     'Relu_46',   0xd5f000,    0x20000,  2),
(                     'Conv_71',   0xd7f000,     0x5000,  2),
(                 'Transpose_3',   0xd7f000,     0x5000,  2),
(              'real_Reshape_7',   0xd7f000,     0x5000,  2),
(                   'Reshape_7',   0xd7f000,     0x5000,  2),
(                     'Relu_45',   0xd84000,    0x20000,  2),
(                     'Conv_69',   0xda4000,     0x5000,  2),
(                 'Transpose_2',   0xda4000,     0x5000,  2),
(              'real_Reshape_1',   0xda4000,     0x5000,  2),
(                   'Reshape_1',   0xda4000,     0x5000,  2),
(                     'Relu_35',   0xda9000,     0x7000,  2),
(                     'Relu_36',   0xdb0000,     0x2000,  2),
(       'BatchNormalization_54',   0xdb2000,     0x4000,  2),
(                     'Relu_48',   0xdb6000,     0x4000,  2),
(                     'Conv_75',   0xdba000,     0x2000,  2),
(                 'Transpose_5',   0xdba000,     0x2000,  2),
(              'real_Reshape_8',   0xdba000,     0x2000,  2),
(                   'Reshape_8',   0xdba000,     0x2000,  2),
(                     'Relu_47',   0xdbc000,     0x4000,  2),
(                     'Conv_73',   0xdc0000,     0x2000,  2),
(                 'Transpose_4',   0xdc0000,     0x2000,  2),
(              'real_Reshape_2',   0xdc0000,     0x2000,  2),
(                   'Reshape_2',   0xdc0000,     0x2000,  2),
(                     'Relu_37',   0xdc2000,     0x1000,  2),
(                     'Relu_38',   0xdc3000,     0x1000,  2),
(       'BatchNormalization_57',   0xdc4000,     0x1000,  2),
(                     'Relu_50',   0xdc5000,     0x1000,  2),
(                     'Conv_79',   0xdc6000,     0x1000,  2),
(                 'Transpose_7',   0xdc6000,     0x1000,  2),
(              'real_Reshape_9',   0xdc6000,     0x1000,  2),
(                   'Reshape_9',   0xdc6000,     0x1000,  2),
(                     'Relu_49',   0xdc7000,     0x1000,  2),
(                     'Conv_77',   0xdc8000,     0x1000,  2),
(                 'Transpose_6',   0xdc8000,     0x1000,  2),
(              'real_Reshape_3',   0xdc8000,     0x1000,  2),
(                   'Reshape_3',   0xdc8000,     0x1000,  2),
(                     'Relu_39',   0xdc9000,     0x1000,  2),
(                     'Relu_40',   0xdca000,     0x1000,  2),
(       'BatchNormalization_60',   0xdcb000,     0x1000,  2),
(                     'Relu_52',   0xdcc000,     0x1000,  2),
(                     'Conv_83',   0xdcd000,     0x1000,  2),
(                 'Transpose_9',   0xdcd000,     0x1000,  2),
(             'real_Reshape_10',   0xdcd000,     0x1000,  2),
(                  'Reshape_10',   0xdcd000,     0x1000,  2),
(                     'Relu_51',   0xdce000,     0x1000,  2),
(                     'Conv_81',   0xdcf000,     0x1000,  2),
(                 'Transpose_8',   0xdcf000,     0x1000,  2),
(              'real_Reshape_4',   0xdcf000,     0x1000,  2),
(                   'Reshape_4',   0xdcf000,     0x1000,  2),
(                     'Relu_41',   0xdd0000,     0x1000,  2),
(                     'Relu_42',   0xdd1000,     0x1000,  2),
(       'BatchNormalization_63',   0xdd2000,     0x1000,  2),
(                     'Conv_85',   0xdd3000,     0x1000,  2),
(                'Transpose_11',   0xdd3000,     0x1000,  2),
(             'real_Reshape_11',   0xdd3000,     0x1000,  2),
(                  'Reshape_11',   0xdd3000,     0x1000,  2),
(                    'Concat_1',        0x0,    0x18000,  4),
(                   'Softmax_0',        0x0,    0x18000,  4),
(                         '740',    0x18000,        0x0,  4),
(                     'Conv_84',   0xdd4000,     0x1000,  2),
(                'Transpose_10',   0xdd4000,     0x1000,  2),
(              'real_Reshape_5',   0xdd4000,     0x1000,  2),
(                   'Reshape_5',   0xdd4000,     0x1000,  2),
(                    'Concat_0',    0x18000,    0x18000,  4),
(                         '726',    0x30000,        0x0,  4),
*/
#endif /*__QUANTIZED_NETWORK_OACT_H__*/
