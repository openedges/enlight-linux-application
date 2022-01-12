
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
    int             softmax_use,       /**< softmax in yolov2           */
    int             num_output,        /**< output loc tensor number    */
    act_tensor_t**  outputs,           /**< output conf tensor number   */
    uint8_t*        def_box_addr,      /**< default box base addresses  */
    float           def_box_scl,       /**< default box scales          */
    uint32_t*       exp_tbl_addr,      /**< output exp(x) tables        */
    uint32_t*       sig_tbl_addr,      /**< output sigmoid(x) tables    */
    int             num_class,         /**< class num                   */
    int*            num_grids,         /**< grid                        */
    int             num_anchor,        /**< anchor num, #box per a grid */
    int*            img_size,          /**< input image H, W            */
    float           th_conf,           /**< confidence threshold        */
    float           th_iou,            /**< IOU threshold               */
    objs_t*         detected_objs      /**< return objects buf base     */
    );


void run_post_process(void *network_instance, void *output_base)
{
    network_t *inst;
    yolo_detector_t* det_param;
    float th_conf;
    float th_iou;
    int num_class;
    int num_box;
    int *img_sizes;
    int *num_grids;
    int loc_scale;
    uint32_t* exp_tables;
    uint32_t* sig_tables;

    int num_output;
    act_tensor_t** output_tensors;

    uint8_t* def_box_addr;
    float def_box_scale;
    objs_t* objs_buf_base;

    int i, softmax_use, num_anchor;

    inst = (network_t *)network_instance;
    det_param = (yolo_detector_t*)inst->post_proc_extension;

    num_output = enlight_yolo_get_output_tensors(
       det_param, &output_tensors);

    for (i = 0; i < num_output; i++)
       output_tensors[i]->base = output_base;

    num_class = enlight_yolo_get_num_class(det_param);

    th_conf = enlight_yolo_get_thres_conf(det_param);

    th_iou = enlight_yolo_get_thres_iou(det_param);

    img_sizes = enlight_yolo_get_img_sizes(det_param);

    num_grids = enlight_yolo_get_num_grids(det_param);

    def_box_scale = enlight_yolo_get_default_box_scale(det_param);

    def_box_addr = enlight_yolo_get_default_box_buf_addr(det_param);

    exp_tables = enlight_yolo_get_output_exp_table(det_param, 0);

    sig_tables = enlight_yolo_get_output_sig_table(det_param, 0);

    objs_buf_base = enlight_yolo_get_objs_buf_base(det_param);

    softmax_use = enlight_yolo_get_softmax_use(det_param);

    num_anchor = enlight_yolo_get_num_anchor(det_param);

    yolo_init();

    yolo_detect_object(
        softmax_use,
        num_output,
        output_tensors,
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

    for (i = 0; i < objs_buf_base->cnt ; i++) {
        obj_t *obj = &objs_buf_base->obj[i];
        post_process_log(
          "min_xy(%4d,%4d) max_xy(%4d,%4d) score: %d%\n",
            obj->x_min, obj->y_min,
            obj->x_max, obj->y_max, obj->class, obj->score);
    }
}