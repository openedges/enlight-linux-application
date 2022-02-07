
/*
    Openedges Enlight Network Compiler
    YOLO detector main
*/

#include "stdint.h"
#include "enlight_network.h"

#ifdef __i386__
#   include <stdio.h>
#   define post_process_log(...) do {printf(__VA_ARGS__);} while(0)
#else
#   define post_process_log(...) do {_printf(__VA_ARGS__);} while(0)
#endif

extern void yolo_init();

extern int yolo_detect_object(
    int             softmax_use,            /**< softmax in yolov2           */
    int             sigmoid_applied,        /**< sigmoid applied to outputs  */
    int             num_loc,                /**< output loc tensor number    */
    int             num_conf,               /**< output conf tensor number   */
    enlight_act_tensor_t**
                    loc_outputs,            /**< output loc tensor number    */
    enlight_act_tensor_t**
                    conf_outputs,           /**< output conf tensor number   */
    uint8_t*        def_box_addr,           /**< default box base addresses  */
    float           def_box_scl,            /**< default box scales          */
    uint32_t*       exp_tbl_addr,           /**< output exp(x) tables        */
    uint32_t*       sig_tbl_addr,           /**< output sigmoid(x) tables    */
    int             num_class,              /**< class num                   */
    int*            num_grids,              /**< grid                        */
    int             num_anchor,             /**< anchor num, #box per a grid */
    int*            img_size,               /**< input image H, W            */
    float           th_conf,                /**< confidence threshold        */
    float           th_iou,                 /**< IOU threshold               */
    enlight_objs_t* detected_objs           /**< return objects buf base     */
    );


/** @brief Run YOLO post-processing and return detected objects
 *
 *      YOLO post-processing, yolo_detect_object() detects objects
 *      from network output tensor data, returns detected objects result
 *      by detected_objects buffer. Caller allocates/frees
 *      detected_objects buffer.
 *
 *  @param network_instance instance Network instance.
 *  @param output_base      output tensor buffer base
 *  @param detected_objects detected objects address.
 *  
 *  @return Return number of detected object
 */
int yolov4_waymo100k_3c_640_TDN_BNH_MIN32CH_quantized_run_post_process(
    void *network_instance,
    void *output_base,
    enlight_objs_t *detected_objects)
{
    enlight_network_t *inst;
    enlight_yolo_detector_t* det_param;
    float th_conf;
    float th_iou;
    int num_class;
    int *img_sizes;
    int out_tensor_divided;
    int *num_grids;
    uint32_t* exp_tables;
    uint32_t* sig_tables;

    int num_loc;
    int num_conf;
    enlight_act_tensor_t** loc_tensors;
    enlight_act_tensor_t** conf_tensors;

    uint8_t* def_box_addr;
    float def_box_scale;
    enlight_objs_t* objs_buf_base;

    int i, softmax_use, sigmoid_applied, num_anchor;

    inst = (enlight_network_t *)network_instance;
    det_param = (enlight_yolo_detector_t*)inst->post_proc_extension;

    num_loc = enlight_yolo_get_loc_tensors(det_param, &loc_tensors);
    num_conf = enlight_yolo_get_conf_tensors(det_param, &conf_tensors);

    for (i = 0; i < num_loc; i++)
       loc_tensors[i]->base = output_base;

    for (i = 0; i < num_conf; i++)
       conf_tensors[i]->base = output_base;

    num_class = enlight_yolo_get_num_class(det_param);

    th_conf = enlight_yolo_get_thres_conf(det_param);

    th_iou = enlight_yolo_get_thres_iou(det_param);

    img_sizes = enlight_get_img_sizes(inst);

    out_tensor_divided = enlight_yolo_get_out_tensor_divided(det_param);

    num_grids = enlight_yolo_get_num_grids(det_param);

    def_box_scale = enlight_yolo_get_default_box_scale(det_param);

    def_box_addr = enlight_yolo_get_default_box_buf_addr(det_param);

    exp_tables = enlight_yolo_get_output_exp_table(det_param, 0);

    sig_tables = enlight_yolo_get_output_sig_table(det_param, 0);

    objs_buf_base = enlight_yolo_get_objs_buf_base(det_param);

    softmax_use = enlight_yolo_get_softmax_use(det_param);

    sigmoid_applied = enlight_yolo_get_sigmoid_applied(det_param);

    num_anchor = enlight_yolo_get_num_anchor(det_param);

    yolo_init();

    yolo_detect_object(
        softmax_use,
        sigmoid_applied,
        num_loc,
        num_conf,
        loc_tensors,
        conf_tensors,
        def_box_addr,
        def_box_scale,
        exp_tables,
        sig_tables,
        num_class,
        num_grids,
        num_anchor,
        img_sizes,
        th_conf,
        th_iou,
        objs_buf_base
    );

    for (i = 0; i < objs_buf_base->cnt; i++) {
        enlight_obj_t *obj = &objs_buf_base->obj[i];

        post_process_log(
            "min_xy(%4d,%4d) max_xy(%4d,%4d) class: %d score: %d%\n",
            obj->x_min, obj->y_min,
            obj->x_max, obj->y_max, obj->cls, obj->score);
    }

    if (detected_objects != 0) {
        detected_objects->cnt = objs_buf_base->cnt;
        for (i = 0; i < objs_buf_base->cnt; i++) {
            enlight_obj_t *obj = &objs_buf_base->obj[i];
            enlight_obj_t *ret_obj = &detected_objects->obj[i];

            ret_obj->x_min = obj->x_min;
            ret_obj->x_max = obj->x_max;
            ret_obj->y_min = obj->y_min;
            ret_obj->y_max = obj->y_max;
            ret_obj->cls   = obj->cls;
            ret_obj->score = obj->score;
            ret_obj->img_w = obj->img_w;
            ret_obj->img_h = obj->img_h;
        }
    }

    return objs_buf_base->cnt;
}