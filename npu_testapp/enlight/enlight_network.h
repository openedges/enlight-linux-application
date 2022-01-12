
#ifndef __ENLIGHT_NETWORK_H__
#define __ENLIGHT_NETWORK_H__
/*
    Openedges Enlight Network Compiler

    Network struct and APIs.
*/

#include "enlight_data_type.h"

#ifdef TEST_ON_EMBEDDED_LINUX
#   define ENLIGHT_NETWORK_PRINT  printf
#else
extern int _printf(const char *format, ...);
#   define ENLIGHT_NETWORK_PRINT  _printf
#endif

#define enlight_network_err(...)  do {ENLIGHT_NETWORK_PRINT(__VA_ARGS__); } while(0)
#define enlight_network_log(...)  do {ENLIGHT_NETWORK_PRINT(__VA_ARGS__); } while(0)

#ifdef ENLIGHT_NETWORK_DEBUG
#   define enlight_network_dbg(...)  do {ENLIGHT_NETWORK_PRINT(__VA_ARGS__); } while(0)
#else
#   define enlight_network_dbg(...)  do {} while(0)
#endif


/** @brief Return network name.
 *
 *      Network name, instance->network_name is file name of quantized enlight.
 *
 *  @param instance Network instance.
 *  @return         String of network name.
 */
char* enlight_get_network_name(network_t *instance);

/** @brief Set network buf bases
 *
 *      ext_base_addr[0] = base[0]; # Code
 *      ext_base_addr[1] = base[1]; # Weight
 *      ext_base_addr[2] = base[2]; # Work
 *      ext_base_addr[3] = base[3]; # Input
 *      ext_base_addr[4] = base[4]; # Output
 *
 *  @param instance Network instance.
 *  @param base_addrs base buffer base[8]
 */
void enlight_set_buf_bases(network_t *instance, void* bases[8]);

/** @brief Get network buf base
 *
 *      ext_base_addr[0]: Code
 *      ext_base_addr[1]: Weight
 *      ext_base_addr[2]: Work
 *      ext_base_addr[3]: Input
 *      ext_base_addr[4]: Output
 *
 *  @param instance Network instance.
 *  @param idx      base index
 *  @return  base_addrs base buffer base
 */
void* enlight_get_buf_base(network_t *instance, int idx);

/** @brief Return int8_t quantized tensor data w/ dims=[H, W, C]
 *
 *      Read quantized output data from source tensor
 *      and write it to target buffer.
 *      
 *      To allocate/free target buffer by Caller.
 *
 *  @param tensor      source tensor
 *  @param quant_data  target buffer
 */
void  enlight_get_tensor_qdata(act_tensor_t *tensor, int8_t *quant_data);

/** @brief Return float tensor data w/ dims=[H, W, C]
 *
 *      Read quantized output data from source tensor,
 *      De-quantize and then write it to target buffer.
 *      
 *      To allocate/free target buffer by Caller.
 *
 *  @param tensor      source tensor
 *  @param quant_data  target buffer
 */
void  enlight_get_tensor_data(act_tensor_t *tensor, float *de_quant_data);

/** @brief Return float tensor data w/ dims=[H, W, C]
 *  @param tensor      source tensor
 *  @return            float tensor's scale
 */
float enlight_get_tensor_scale(act_tensor_t *tensor);

/** @brief Return sigle int8_t quantized tensor data
 *
 *      Return quantized_data[y_off, x_off, c_off]
 *
 *  @param tensor      source tensor
 *  @param y_off       offset in height
 *  @param x_off       offset in width
 *  @param c_off       offset in channel
 *  @return            quantized data        
 */
int8_t enlight_get_tensor_qdata_by_index(act_tensor_t *tensor, int y_off, int x_off, int c_off);

/** @brief Return single float a tensor data
 *
 *      Return quantized_data[y_off, x_off, c_off] / scale
 *
 *  @param tensor      source tensor
 *  @param y_off       offset in height
 *  @param x_off       offset in width
 *  @param c_off       offset in channel
 *  @return            de-quantized data        
 */
float enlight_get_tensor_data_by_index(act_tensor_t *tensor, int y_off, int x_off, int c_off);

/** @brief Return tensor data size H x W x C
 *
 *  @param tensor      tensor
 *  @return            size
 */
int   enlight_get_qtensor_size(act_tensor_t *tensor);

/** @brief Return tensor data type
 *
 *  @param tensor      tensor
 *  @return data type in enlight_data_t
 */
int enlight_get_tensor_quant_data_type(act_tensor_t *tensor);

/** @brief Return code/weight/input/output/work buffer size
 *
 *  @param instance Network instance.
 *  @return         byte size
 */
int enlight_get_code_buf_size(network_t *instance);
int enlight_get_weight_buf_size(network_t *instance);
int enlight_get_input_buf_size(network_t *instance);
int enlight_get_output_buf_size(network_t *instance);
int enlight_get_work_buf_size(network_t *instance);

// APIs only for ENLIGHT toolkit developer
uint8_t* enlight_get_code_base(network_t *instance);


/*************************************************************************/
/* SSD post process extension APIs                                       */
/*************************************************************************/

/** @brief Return float type value of Confidence threshold
 *
 *  @param  param ssd_detector instance address
 *  @return       float type value
 */
float enlight_ssd_get_thres_conf(ssd_detector_t* param);

/** @brief Return float type value of IOU threshold
 *
 *  @param  param ssd_detector instance address
 *  @return       float type value
 */
float enlight_ssd_get_thres_iou(ssd_detector_t* param);

/** @brief Return class number
 *
 *  @param  param ssd_detector instance address
 *  @return       class number
 */
int enlight_ssd_get_num_class(ssd_detector_t* param);

/** @brief Return box number
 *
 *  @param  param ssd_detector instance address
 *  @return       box number
 */
int enlight_ssd_get_num_box(ssd_detector_t* param);

/** @brief Return input image H, W
 *
 *  @param  param ssd_detector instance address
 *  @return img_sizes address
 */
int* enlight_ssd_get_img_sizes(ssd_detector_t* param);

/** @brief Return default box scale_1024
 *
 *  @param  param ssd_detector instance address
 *  @return       scale
 */
float enlight_ssd_get_default_box_scale(ssd_detector_t* ssd_param);

/** @brief Return default box buffer addr
 *
 *  @param  param ssd_detector instance address
 *  @return       buffer addr
 */
uint8_t* enlight_ssd_get_default_box_buf_addr(ssd_detector_t* param);

/** @brief Return default box address
 *      Process as followings steps.
 *          0. Allocate buffer, float buf[num_box];
 *          1. buf[i] = (float)default_box[i] / (1 << default_box_scale);
 *
 *      Alloc/free buffer by Caller
 *
 *  @param  param ssd_detector instance address
 *  @param  buf   float type buffer
 */
void      enlight_ssd_get_float_default_boxes(ssd_detector_t* param, float* buf);

float     enlight_ssd_get_conf_scales(ssd_detector_t* param, int idx);
float     enlight_ssd_get_loc_scales(ssd_detector_t* param, int idx);

uint32_t* enlight_ssd_get_conf_exp_table(ssd_detector_t* param, int idx);
uint32_t* enlight_ssd_get_loc_exp_table(ssd_detector_t* param, int idx);
objs_t*   enlight_ssd_get_objs_buf_base(ssd_detector_t* param);

/** @brief Return loc tensors addr
 *
 *  @param  param       ssd_detector instance address
 *  @param  tensor_buf  tensor base return buffer
 *  @return       tensors number
 */
int enlight_ssd_get_loc_tensors(ssd_detector_t* param, act_tensor_t ***tensor_buf);

/** @brief Return conf tensors addr
 *
 *  @param  param       ssd_detector instance address
 *  @param  tensor_buf  tensor base return buffer
 *  @return       tensors number
 */
int enlight_ssd_get_conf_tensors(ssd_detector_t* param, act_tensor_t ***tensor_buf);

/*************************************************************************/
/* YOLO post process extension APIs                                      */
/*************************************************************************/
int       enlight_yolo_get_softmax_use(yolo_detector_t* p);
int       enlight_yolo_get_output_tensors(yolo_detector_t* p, act_tensor_t ***tensor_buf);
float     enlight_yolo_get_thres_conf(yolo_detector_t* p);
float     enlight_yolo_get_thres_iou(yolo_detector_t* p);
int       enlight_yolo_get_num_class(yolo_detector_t* p);
int*      enlight_yolo_get_num_grids(yolo_detector_t* p);
int       enlight_yolo_get_num_anchor(yolo_detector_t* p);
int*      enlight_yolo_get_img_sizes(yolo_detector_t* p);
float     enlight_yolo_get_default_box_scale(yolo_detector_t* p);
uint8_t*  enlight_yolo_get_default_box_buf_addr(yolo_detector_t* p);
void      enlight_yolo_get_float_default_boxes(yolo_detector_t* p, float* buf);
float     enlight_yolo_get_output_scales(yolo_detector_t* p, int output_index);
uint32_t* enlight_yolo_get_output_exp_table(yolo_detector_t* p, int output_index);
uint32_t* enlight_yolo_get_output_sig_table(yolo_detector_t* p, int output_index);
objs_t*   enlight_yolo_get_objs_buf_base(yolo_detector_t* p);

#endif /*__ENLIGHT_NETWORK__*/
