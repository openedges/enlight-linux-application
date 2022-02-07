/**
 *  Openedges Enlight NPU Testapplication
 *
 *  Post Process.
*/
#ifndef __POST_PROCESS_H__
#define __POST_PROCESS_H__

#include "yolov4_waymo100k_3c_640_TDN_BNH_MIN32CH_quantized_post_process.c"

int (*run_post_process)(void *, void *,void*) = yolov4_waymo100k_3c_640_TDN_BNH_MIN32CH_quantized_run_post_process;

#endif
