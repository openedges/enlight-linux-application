

#ifndef __PYTORCH_MOBILENET_V2_QUANTIZED_NETWORK_H__
#define __PYTORCH_MOBILENET_V2_QUANTIZED_NETWORK_H__

/*
    Openedges Enlight Network Compiler
*/

#include "enlight_data_type.h"
#include "enlight_network.h"
#ifdef BARE_METAL_FW_DEV
#    include "npu_cmd.h"
#endif


static enlight_act_tensor_t tensor_Conv_14 = {
    .name = "Conv_14",
    .data_type = ENLIGHT_DTYPE_INT8,
    .base = (void*)0,
    .buf = 0x0,
    .size = 100352,
    .fb = 2,
    .num_dimension = 4,
    .dimensions = {1, 24, 56, 56},
    .scale = 22.9236,
    .zp = 0,
    .output = 0,
};

static enlight_act_tensor_t tensor_Conv_57 = {
    .name = "Conv_57",
    .data_type = ENLIGHT_DTYPE_INT8,
    .base = (void*)0,
    .buf = 0x18800,
    .size = 12544,
    .fb = 2,
    .num_dimension = 4,
    .dimensions = {1, 64, 14, 14},
    .scale = 38.4878,
    .zp = 0,
    .output = 0,
};

static enlight_act_tensor_t tensor_Conv_118 = {
    .name = "Conv_118",
    .data_type = ENLIGHT_DTYPE_INT8,
    .base = (void*)0,
    .buf = 0x1b900,
    .size = 7840,
    .fb = 2,
    .num_dimension = 4,
    .dimensions = {1, 160, 7, 7},
    .scale = 53.8055,
    .zp = 0,
    .output = 0,
};

static enlight_act_tensor_t tensor_Conv_141 = {
    .name = "Conv_141",
    .data_type = ENLIGHT_DTYPE_INT8,
    .base = (void*)0,
    .buf = 0x1d7a0,
    .size = 47040,
    .fb = 2,
    .num_dimension = 4,
    .dimensions = {1, 960, 7, 7},
    .scale = 68.0621,
    .zp = 0,
    .output = 0,
};

static enlight_act_tensor_t tensor_Conv_144 = {
    .name = "Conv_144",
    .data_type = ENLIGHT_DTYPE_INT8,
    .base = (void*)0,
    .buf = 0x28f60,
    .size = 15680,
    .fb = 2,
    .num_dimension = 4,
    .dimensions = {1, 320, 7, 7},
    .scale = 59.0659,
    .zp = 0,
    .output = 0,
};

static enlight_act_tensor_t tensor_Conv_146 = {
    .name = "Conv_146",
    .data_type = ENLIGHT_DTYPE_INT8,
    .base = (void*)0,
    .buf = 0x2cca0,
    .size = 62720,
    .fb = 2,
    .num_dimension = 4,
    .dimensions = {1, 1280, 7, 7},
    .scale = 21.1667,
    .zp = 0,
    .output = 0,
};

static enlight_act_tensor_t tensor_GlobalAveragePool_149 = {
    .name = "GlobalAveragePool_149",
    .data_type = ENLIGHT_DTYPE_INT8,
    .base = (void*)0,
    .buf = 0x3c1a0,
    .size = 1280,
    .fb = 2,
    .num_dimension = 4,
    .dimensions = {1, 1280, 1, 1},
    .scale = 21.1667,
    .zp = 0,
    .output = 0,
};

static enlight_act_tensor_t tensor_Gemm_151 = {
    .name = "Gemm_151",
    .data_type = ENLIGHT_DTYPE_INT8,
    .base = (void*)0,
    .buf = 0x0,
    .size = 1024,
    .fb = 4,
    .num_dimension = 4,
    .dimensions = {1, 1000, 1, 1},
    .scale = 4.1810,
    .zp = 0,
    .output = 1,
};

/* Buf description of Activation and Output*/
static enlight_act_tensor_t* all_output_tensors[8] = {
    &tensor_Conv_14,
    &tensor_Conv_57,
    &tensor_Conv_118,
    &tensor_Conv_141,
    &tensor_Conv_144,
    &tensor_Conv_146,
    &tensor_GlobalAveragePool_149,
    &tensor_Gemm_151,
};


static enlight_classification_t post_ext_param = {
    .num_class         = 1000,
    .output_tensors    = {&tensor_Gemm_151,},
    .output_tensors_num= 1,
};

static enlight_buf_size_t network_buf_size = {
    .code_buf_size         = 0x2e4c, //11852
    .wght_buf_size         = 0x362a00, //3549696
    .input_buf_size        = 0x24c00, //150528
    .output_buf_size       = 0x400, //1024
    .work_buf_size         = 0x3c6a0, //247456
};

enlight_network_t pytorch_mobilenet_v2_quantized_network_instance = {

    .network_name          = "pytorch_mobilenet_v2_quantized.enlight",

    .post_proc             = ENLIGHT_POST_CLASSIFIER,

    .img_sizes             = {3, 224, 224},

    .output_tensors_num    = 1,
    .output_tensors        = {&tensor_Gemm_151,},

    .buf_sizes             = &network_buf_size,

    .post_proc_extension   = (void*)&post_ext_param,

    // For Debug
    .all_tensors           = all_output_tensors,

    // For toolkit developer
#ifdef BARE_METAL_FW_DEV
    .npu_cmd_codes         = npu_cmd,
#endif
};

#endif /*__PYTORCH_MOBILENET_V2_QUANTIZED_NETWORK_H__*/