/*
    Openedges Enlight Network Compiler

    Network APIs functions.
*/

#include "enlight_network.h"


char* enlight_get_network_name(network_t *instance) 
{
    return instance->network_name;
}

void enlight_set_buf_bases(network_t *instance, void* bases[8])
{
    int i;
    for (i = 0; i < 8; i++)
        instance->ext_base_addr[i] = bases[i];
}

void* enlight_get_buf_base(network_t *instance, int idx)
{
    return instance->ext_base_addr[idx];
}

void enlight_get_tensor_qdata(act_tensor_t *tensor, int8_t *dst)
{
    int i;

    enlight_network_dbg("tensor %08x\n", tensor);
    enlight_network_dbg("base   %08x\n", tensor->base);
    enlight_network_dbg("off    %08x\n", tensor->buf);

    int8_t* src = (int8_t*)(tensor->base + (unsigned long)tensor->buf);

    enlight_network_dbg("src %08x\n", src);

    if (tensor->output) {
        for (i = 0 ; i < tensor->size; i++)
            *dst++ = (int8_t)src[i];
    }
    else {
        const int WORD_SIZE = 32; 

        int ii, jj, kk;
        int c, h, w;

        c = tensor->dimensions[1];
        w = tensor->dimensions[2];
        h = tensor->dimensions[3];

        for(ii = 0; ii < h; ii++) {
            for(jj = 0; jj < w; jj++)
                for(kk = 0; kk < c; kk++) {

                    i = (kk % WORD_SIZE);
                    i += (WORD_SIZE * jj);
                    i += (WORD_SIZE * w * ii);
                    i += ((kk / WORD_SIZE) * h * w); 

                    *dst++ = (int8_t)src[i];
                }
        }
    }
}

void enlight_get_tensor_data(act_tensor_t *tensor, float *dst)
{
    int i;

    enlight_network_dbg("tensor %08x\n", tensor);
    enlight_network_dbg("base   %08x\n", tensor->base);
    enlight_network_dbg("off    %08x\n", tensor->buf);

    int8_t* src = (int8_t*)(tensor->base + (unsigned long)tensor->buf);

    enlight_network_dbg("src %08x\n", src);

    if (tensor->output) {
        for (i = 0 ; i < tensor->size; i++)
            *dst++ = (float)src[i] * 1024 / tensor->scale_1024;
    }
    else {
        const int WORD_SIZE = 32; 

        int ii, jj, kk;
        int c, h, w;

        c = tensor->dimensions[1];
        w = tensor->dimensions[2];
        h = tensor->dimensions[3];

        for(ii = 0; ii < h; ii++) {
            for(jj = 0; jj < w; jj++)
                for(kk = 0; kk < c; kk++) {

                    i = (kk % WORD_SIZE);
                    i += (WORD_SIZE * jj);
                    i += (WORD_SIZE * w * ii);
                    i += ((kk / WORD_SIZE) * h * w); 

                    *dst++ = (float)src[i] * 1024 / tensor->scale_1024;
                }
        }
    }
}

float enlight_get_tensor_scale(act_tensor_t *tensor)
{
    return (float)tensor->scale_1024 / 1024;
}

int8_t enlight_get_tensor_qdata_by_index(act_tensor_t *tensor, int y_off, int x_off, int c_off)
{
    const int WORD_SIZE = 32; 
    int idx;
    int8_t* src = (int8_t*)(tensor->base + (unsigned long)tensor->buf);

    int* dims = tensor->dimensions;

    int c = (dims[1] + (WORD_SIZE - 1)) & 0xFFE0;
    int h = dims[2];
    int w = dims[3];

    if (tensor->output) {
        idx = c_off + (c * x_off) + (c * w * y_off); 
    }
    else {
        idx = (c_off % WORD_SIZE) + (WORD_SIZE * x_off) + (WORD_SIZE * w * y_off) + ((c_off / WORD_SIZE) * h * w); 
    }

    enlight_network_dbg("output %3d\n", tensor->output);
    enlight_network_dbg("dims %3d %3d %3d %3d\n", dims[0], dims[1], dims[2], dims[3]);
    enlight_network_dbg("hwc  %3d %3d %3d\n", h, w, c);
    enlight_network_dbg("off y x c  %3d %3d %3d\n", y_off, x_off, c_off);
    enlight_network_dbg("idx %d\n", idx);
    enlight_network_dbg("base offset 0x%08x + 0x%08x = 0x%08x\n", src, (unsigned long)tensor->buf, &src[idx]);

    return src[idx];
}

float enlight_get_tensor_data_by_index(act_tensor_t *tensor, int y_off, int x_off, int c_off)
{
    float src;
    src = (float)enlight_get_tensor_qdata_by_index(tensor, y_off, x_off, c_off);
    return src * 1024 / tensor->scale_1024;
}

int enlight_get_qtensor_size(act_tensor_t *tensor)
{
    return tensor->size;
}

int enlight_get_tensor_quant_data_type(act_tensor_t *tensor)
{
    return tensor->data_type;
}

int enlight_get_code_buf_size(network_t *instance)
{
    return instance->buf_sizes->code_buf_size;
}

int enlight_get_weight_buf_size(network_t *instance)
{
    return instance->buf_sizes->wght_buf_size;
}

int enlight_get_input_buf_size(network_t *instance)
{
    return instance->buf_sizes->input_buf_size;
}

int enlight_get_output_buf_size(network_t *instance)
{
    return instance->buf_sizes->output_buf_size;
}

int enlight_get_work_buf_size(network_t *instance)
{
    return instance->buf_sizes->work_buf_size;
}

// APIs only for ENLIGHT toolkit developer
uint8_t* enlight_get_code_base(network_t *instance)
{
    return (uint8_t*)instance->npu_cmd_codes;
}


/******************************************************************************/
/*  SSD post process extension APIs                                           */
/******************************************************************************/
float enlight_ssd_get_thres_conf(ssd_detector_t* param) 
{
    float th_conf = param->th_conf;
    float scale = 256.;

    return th_conf / scale;
}

float enlight_ssd_get_thres_iou(ssd_detector_t* param) 
{
    float th_conf = param->th_iou;
    float scale = 256.;

    return th_conf / scale;
}

int enlight_ssd_get_num_class(ssd_detector_t* param) 
{
    return param->num_class;
}

int enlight_ssd_get_num_box(ssd_detector_t* param) 
{
    return param->num_box;
}

int* enlight_ssd_get_img_sizes(ssd_detector_t* param) 
{
    return param->img_size;
}

float enlight_ssd_get_default_box_scale(ssd_detector_t* param)
{
    return param->default_box_scale;
}

uint8_t* enlight_ssd_get_default_box_buf_addr(ssd_detector_t* param)
{
    return param->default_box;
}

void enlight_ssd_get_float_default_boxes(ssd_detector_t* param, float* buf)
{
    int i;
    int num_box = param->num_box;
    float scale = param->default_box_scale;
    uint8_t* default_box = param->default_box;

    for (i=0; i < num_box; i++)
    {
        buf[i*4] = ((float)default_box[i*4]) / scale;
    }
}

float enlight_ssd_get_conf_scales(ssd_detector_t* param, int idx)
{
    act_tensor_t* out_tensor = param->conf_tensors[idx];
    return (float)out_tensor->scale_1024 / 1024;
}

float enlight_ssd_get_loc_scales(ssd_detector_t* param, int idx)
{
    act_tensor_t* out_tensor = param->loc_tensors[idx];
    return (float)out_tensor->scale_1024 / 1024;
}

uint32_t* enlight_ssd_get_conf_exp_table(ssd_detector_t* param, int output_index)
{
    return &param->conf_exp_tables[output_index * 256];
}

objs_t*   enlight_ssd_get_objs_buf_base(ssd_detector_t* param)
{
    return &param->detected_objects;
}

uint32_t* enlight_ssd_get_loc_exp_table(ssd_detector_t* param, int output_index)
{
    return &param->loc_exp_tables[output_index * 256];
}

int enlight_ssd_get_loc_tensors(ssd_detector_t* param, act_tensor_t ***tensor_buf)
{
    *tensor_buf = param->loc_tensors;
    return param->loc_tensors_num;
}

int enlight_ssd_get_conf_tensors(ssd_detector_t* param, act_tensor_t ***tensor_buf)
{
    *tensor_buf = param->conf_tensors;
    return param->conf_tensors_num;
}

/******************************************************************************/
/*  YOLO post process extension APIs                                           */
/******************************************************************************/
int enlight_yolo_get_softmax_use(yolo_detector_t* param)
{
    return param->softmax_use;
}

int enlight_yolo_get_output_tensors(yolo_detector_t* param, act_tensor_t ***tensor_buf)
{
    *tensor_buf = param->output_tensors;
    return param->output_tensors_num;
}

float enlight_yolo_get_thres_conf(yolo_detector_t* param) 
{
    float th_conf = param->th_conf;
    float scale = 256.;

    return th_conf / scale;
}

float enlight_yolo_get_thres_iou(yolo_detector_t* param) 
{
    float th_conf = param->th_iou;
    float scale = 256.;

    return th_conf / scale;
}

int enlight_yolo_get_num_class(yolo_detector_t* param) 
{
    return param->num_class;
}

int* enlight_yolo_get_num_grids(yolo_detector_t* param) 
{
    return param->num_grids;
}

int enlight_yolo_get_num_anchor(yolo_detector_t* param) 
{
    return param->num_anchor;
}

int* enlight_yolo_get_img_sizes(yolo_detector_t* param)
{
    return param->img_size;
}

float enlight_yolo_get_default_box_scale(yolo_detector_t* param)
{
    return param->default_box_scale;
}

uint8_t* enlight_yolo_get_default_box_buf_addr(yolo_detector_t* param)
{
    return param->default_box;
}

void enlight_yolo_get_float_default_boxes(yolo_detector_t* param, float* buf)
{
    int i;
    float scale = param->default_box_scale;
    uint8_t* default_box = param->default_box;
    int num_box;

    num_box = 0;
    for (i=0; i < param->output_tensors_num; i++) {
        num_box += param->num_grids[i * 2] * param->num_grids[(i * 2) + 1];
    }

    for (i=0; i < num_box; i++)
    {
        buf[i*4] = ((float)default_box[i*4]) / scale;
    }
}

float enlight_yolo_get_output_scales(yolo_detector_t* param, int idx)
{
    act_tensor_t* out_tensor = param->output_tensors[idx];
    return (float)out_tensor->scale_1024 / 1024;
}

uint32_t* enlight_yolo_get_output_exp_table(yolo_detector_t* param, int output_index)
{
    return &param->exp_tables[output_index * 256];
}

uint32_t* enlight_yolo_get_output_sig_table(yolo_detector_t* param, int output_index)
{
    return &param->sig_tables[output_index * 256];
}

objs_t*   enlight_yolo_get_objs_buf_base(yolo_detector_t* param)
{
    return &param->detected_objects;
}
