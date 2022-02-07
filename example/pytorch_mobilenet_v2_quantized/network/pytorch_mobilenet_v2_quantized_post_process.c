
/*
    Openedges Enlight Network Compiler
    Classification main
*/

#include "stdint.h"
#include "enlight_network.h"

#ifdef __i386__
#   include <stdio.h>
#   define post_process_log(...) do {printf(__VA_ARGS__);} while(0)
#else
#   define post_process_log(...) do {_printf(__VA_ARGS__);} while(0)
#endif

extern void classifier_init();

extern int classifier_run(
    enlight_act_tensor_t*   outputs,           /**< output conf tensor number   */
    int                     num_class          /**< class num                   */
    );


/** @brief Run Classification post-processing and return top-1 class ID.
 *
 *      Classification post-processing, classifier_run() finds and returns
 *      top-1 class ID from network output tensor data.
 *
 *  @param network_instance instance Network instance.
 *  @param output_base      output tensor buffer base
 *  @param reserved 
 *
 *  @return Return number of detected object
 */
int pytorch_mobilenet_v2_quantized_run_post_process(
    void *network_instance,
    void *output_base,
    void *reserved)
{
    enlight_network_t *inst;
    enlight_classification_t* cls_param;
    int num_class;
    int class_id ;

    int i, num_output;
    enlight_act_tensor_t** output_tensors;

    inst = (enlight_network_t *)network_instance;
    cls_param = (enlight_classification_t*)inst->post_proc_extension;

    num_output = enlight_class_get_output_tensors(cls_param, &output_tensors);

    for (i = 0; i < num_output; i++)
        output_tensors[i]->base = output_base;

    num_class = enlight_class_get_num_class(cls_param);

    classifier_init();

    class_id = classifier_run(output_tensors[0], num_class);
    post_process_log("class: %d\n", class_id);

    return class_id;
}