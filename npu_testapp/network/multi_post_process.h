
/**
 *  Openedges Enlight NPU Testapplication
 *
 *  multi network Post Process.
*/
#ifndef __MULTI_NETWORK_POST_PROCESS_H__
#define __MULTI_NETWORK_POST_PROCESS_H__
#include "../npu.h"
#if (MULTI)

#include "ssdlite300_quantized_post_process.c"

int (*multi_post_process)(void*,void*,void* ) = ssdlite300_quantized_run_post_process;
#endif
#endif 
