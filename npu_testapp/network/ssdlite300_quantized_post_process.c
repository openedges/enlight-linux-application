
/*
    Openedges Enlight Network Compiler
    SSD detector main
*/

#include "stdint.h"
#include "enlight_network.h"

#ifdef __i386__
#   include <stdio.h>
#   define post_process_log(...) do {printf(__VA_ARGS__);} while(0)
#else
#   define post_process_log(...) do {_printf(__VA_ARGS__);} while(0)
#endif

extern void ssd_init();

extern void ssd_detect_object(
    int                 num_loc,              /**< loc tensor number    */
    int                 num_conf,             /**< conf tensor number   */
    enlight_act_tensor_t** loc_tensors,       /**< loc tensors          */
    enlight_act_tensor_t** conf_tensors,      /**< conf tnesors         */
    uint8_t*            def_box_addrs,        /**< default box base     */
    float               def_box_scales,       /**< default box scales   */
    uint32_t*           loc_exp_tables,       /**< conf exp(x) tables   */
    uint32_t*           con_logistic_tables,  /**< loc exp(x) tables    */
    int                 num_class,            /**< number of class      */
    int                 num_box,              /**< number of box        */
    int*                img_sizes,            /**< input image H, W     */
    float               th_conf,              /**< confidence threshold */
    float               th_iou,               /**< IOU threshold        */
    int                 bg_in_cls,            /**< background in class  */
    enlight_logistic_t  logistic,             /**< logstic type         */
    enlight_nms_t       nms_method,           /**< nms method  type     */
    enlight_objs_t*     detected_objs         /**< objects buf base     */
    );


/** @brief Run SSD post-processing and return detected objects
 *
 *      SSD post-processing, ssd_detect_object() detects objects
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
int ssdlite300_quantized_run_post_process(
    void *network_instance,
    void *output_base,
    enlight_objs_t *detected_objects)
{
    enlight_network_t *inst;
    enlight_ssd_detector_t* det_param;
    float th_conf;
    float th_iou;
    int num_class;
    int num_box;
    int* img_sizes;
    int bg_in_cls;
    enlight_logistic_t logistic;
    enlight_nms_t nms_method;
    uint32_t* con_logistic_tables;
    uint32_t* loc_exp_tables;

    int num_loc;
    int num_conf;
    enlight_act_tensor_t **loc_tensors;
    enlight_act_tensor_t **conf_tensors;

    uint8_t* def_box_addrs;
    float def_box_scales;
    enlight_objs_t* objs_buf_base;

    int i;

    inst = (enlight_network_t *)network_instance;
    det_param = (enlight_ssd_detector_t*)inst->post_proc_extension;

    num_loc = enlight_ssd_get_loc_tensors(det_param, &loc_tensors);
    num_conf = enlight_ssd_get_conf_tensors( det_param, &conf_tensors);

    for (i = 0; i < num_loc; i++)
       loc_tensors[i]->base = output_base;

    for (i = 0; i < num_conf; i++)
       conf_tensors[i]->base = output_base;

    num_class = enlight_ssd_get_num_class(det_param);

    th_conf = enlight_ssd_get_thres_conf(det_param);

    th_iou = enlight_ssd_get_thres_iou(det_param);

    num_box = enlight_ssd_get_num_box(det_param);

    img_sizes = enlight_get_img_sizes(inst);

    def_box_scales = enlight_ssd_get_default_box_scale(det_param);

    def_box_addrs = enlight_ssd_get_default_box_buf_addr(det_param);

    loc_exp_tables = enlight_ssd_get_loc_exp_table(det_param, 0);

    con_logistic_tables = enlight_ssd_get_conf_logistic_table(det_param, 0);

    bg_in_cls = enlight_ssd_get_bg_in_cls(det_param);

    logistic = enlight_ssd_get_logistic(det_param);

    nms_method = enlight_ssd_get_nms_method(det_param);

    objs_buf_base = enlight_ssd_get_objs_buf_base(det_param);

    ssd_init();

    ssd_detect_object(
        num_loc,
        num_conf,
        loc_tensors,
        conf_tensors,
        def_box_addrs,
        def_box_scales,
        loc_exp_tables,
        con_logistic_tables,
        num_class,
        num_box,
        img_sizes,
        th_conf,
        th_iou,
        bg_in_cls,
        logistic,
        nms_method,
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