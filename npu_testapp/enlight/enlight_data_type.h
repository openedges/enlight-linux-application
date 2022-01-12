
#ifndef __ENLIGHT_DATA_TYPE_H__
#define __ENLIGHT_DATA_TYPE_H__

/**
 *  Openedges Enlight Network Compiler
 *
 *  Enlight Data types definition
*/

#include <stdio.h>
#include <stdint.h>

#define MAX_NUM_OUTPUT      32      /**< Max number of output layers    */
#define MAX_NUM_LOC         8       /**< Max number of ssd loc layers   */
#define MAX_NUM_CONF        8       /**< Max number of ssd conf layers  */
#define MAX_DETECT_BOX      256     /**< max detected box num           */
#define MAX_TENSOR_NAME_LEN 33     /**< Max number of output layers     */


/** @brief A struturce of Enlight data type
 */
typedef enum {
    ENLIGHT_DTYPE_UINT4,        /**< unsigned integer 4 bits        */
    ENLIGHT_DTYPE_INT4,         /**< signed integer 4 bits          */
    ENLIGHT_DTYPE_UINT8,        /**< unsigned integer 8 bits        */
    ENLIGHT_DTYPE_INT8,         /**< signed integer 8 bits          */
    ENLIGHT_DTYPE_UINT16,       /**< unsigned integer 16 bits       */
    ENLIGHT_DTYPE_INT16,        /**< signed integer 16 bits         */
    ENLIGHT_DTYPE_UINT32,       /**< unsigned integer 32 bits       */
    ENLIGHT_DTYPE_INT32,        /**< signed integer 32 bits         */
    ENLIGHT_DTYPE_FP16,         /**< float 16 bits                  */
    ENLIGHT_DTYPE_FP32,         /**< float 32 bits                  */
    ENLIGHT_DTYPE_FLOAT,        /**< float                          */
} enlight_data_t;


/** @brief A struturce to represent weight tensor of layer
 */
typedef struct {
    int     data_type;          /**< data type of weight and bias   */
    void*   wbuf;               /**< base address of weight         */
    int     wsize;              /**< size of weight                 */
    int     fb;                 /**< addr buffer index of weight    */
    void*   bbuf;               /**< base address of bias           */
    int     bsize;              /**< size of bias                   */
} weight_tensor_t;


/** @brief A struturce to represent output/activation tensor of layer
 *  How to get float type output
 *      val = (float)(buf[i] + zp) / ((float)scale_1024 / 1024)
 */
typedef struct {
    int     data_type;          /**< data type of tensor            */
    void*   base;               /**< base address of tensor         */
    void*   buf;                /**< offset address of tensor       */
    int     size;               /**< size of tensor                 */
    int     fb;                 /**< addr buffer index of tensor    */
    int     num_dimension;      /**< number of dimension            */
    int     dimensions[4];      /**< dimensions of tensor           */
    int     scale_1024;         /**< scale_1024 = scale * 1024      */
    float   scale;              /**< float scale                    */
    int     zp;                 /**< zero point                     */
    int     output;             /**< output flag.                   */
    char    name[MAX_TENSOR_NAME_LEN];/**< tensor name              */
} act_tensor_t;

/** @brief A struturce of external buffer sizes.
 */
typedef struct {
    int     code_buf_size;      /**< npu code buf size              */
    int     wght_buf_size;      /**< quantized weight buffer size   */
    int     input_buf_size;     /**< input buffer size              */
    int     output_buf_size;    /**< output buffer size             */
    int     work_buf_size;      /**< temporal buffer size           */
} buf_size_t;


/** @brief Description of a detected object.
 */
typedef struct {
    int16_t x_min;              /**< object box position            */
    int16_t x_max;              /**< object box position            */
    int16_t y_min;              /**< object box position            */
    int16_t y_max;              /**< object box position            */
    int16_t class;              /**< object class                   */
    int16_t score;              /**< object confidence score        */
    int16_t img_w;              /**< input image width              */
    int16_t img_h;              /**< input image height             */
} obj_t;

/** @brief All detected objects.
 */
typedef struct {
    int     cnt;                /**< number of detected objects     */
    obj_t   obj[MAX_DETECT_BOX];/**< detected objects description   */
} objs_t;


typedef struct {
    int           th_conf;                    /**< float conf_thresold = th_conf / 256                   */
    int           th_iou;                     /**< float iou_thresold = th_iou / 256                     */
    int           num_class;                  /**< number of class          */
    int           num_box;                    /**< number of default box    */
    int           num_anchor;                 /**< anchor number per a grid */
    int           img_size[2];                /**< input image size H, W    */
    float         default_box_scale;          /**< float box_val[i] = default_box[i] / default_box_scale */
    uint8_t*      default_box;                /**< default box address      */

    float         loc_scales[MAX_NUM_LOC];    /**< loc scales               */
    float         conf_scales[MAX_NUM_CONF];  /**< confidence scales        */
    uint32_t*     loc_exp_tables;             /**< loc exp tables           */
    uint32_t*     conf_exp_tables;            /**< confidence exp tables    */
    int           loc_tensors_num;            /**< loc tensor num           */
    int           conf_tensors_num;           /**< loc tensor num           */
    act_tensor_t* loc_tensors[MAX_NUM_LOC];   /**< loc tensors              */
    act_tensor_t* conf_tensors[MAX_NUM_CONF]; /**< conf tensors             */
    objs_t        detected_objects;           /**< SSD detected objects     */
} ssd_detector_t;

typedef struct {
    int           softmax_use;                /**< softmax used in Yolov2   */
    int           th_conf;                    /**< float conf_thresold = th_conf / 256                   */
    int           th_iou;                     /**< float iou_thresold = th_iou / 256                     */
    int           num_class;                  /**< number of class          */
    int           img_size[2];                /**< input image size H, W    */
    int           num_grids[3*2];             /**< grid num for 3 stages    */
    int           num_anchor;                 /**< anchor num               */
    float         default_box_scale;          /**< float box_val[i] = default_box[i] / default_box_scale */
    uint8_t*      default_box;                /**< default box address      */
    float         output_scales[MAX_NUM_OUTPUT];    /**< loc scales         */
    uint32_t*     exp_tables;                 /**< exp tables               */
    uint32_t*     sig_tables;                 /**< sigmoid tables           */
    int           output_tensors_num;         /**< output tensor num        */
    act_tensor_t* output_tensors[MAX_NUM_OUTPUT];/**< output tensors        */
    objs_t        detected_objects;           /**< YOLO detected objects    */
} yolo_detector_t;

typedef struct {
    char*           network_name;             /**< Network name             */
    buf_size_t*     buf_sizes;                /**< External memory size info*/
    act_tensor_t**  all_tensors;              /**< all tensors output table */
    void*           ext_base_addr[8];         /**< External base addresses  */
    void*           post_proc_extension;      /**< Post process extension   */
    act_tensor_t*   output_tensors[MAX_NUM_OUTPUT];  /**< output tensors    */

    uint32_t*       npu_cmd_codes;            /**< Only For ENLIGHT toolkit developer
                                                   NPU command code words   */
} network_t;


#endif /*__ENLIGHT_DATA_TYPE_H__*/
