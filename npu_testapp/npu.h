#ifndef NPU_H
#define NPU_H
#include <linux/ioctl.h>

#define POST_NMS	1
#define POST_CLASS	2

#define NPU_IOCTL_MAGIC      'k'
 
#define NPU_IOCTL_RUN               _IO(NPU_IOCTL_MAGIC, 0)

#define NPU_IOCTL_MAX        1

#endif
