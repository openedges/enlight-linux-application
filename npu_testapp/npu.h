#ifndef NPU_H
#define NPU_H
#include <linux/ioctl.h>

struct npu_buf_info {
    char *buffer;
    unsigned int size;
};

#define NPU_IOCTL_MAGIC      'k'
 
#define NPU_IOCTL_RUN               _IO(NPU_IOCTL_MAGIC, 0)
#define NPU_IOCTL_SET_CMD_BUF       _IOW(NPU_IOCTL_MAGIC, 1, struct npu_buf_info)
#define NPU_IOCTL_SET_WEIGHT_BUF    _IOW(NPU_IOCTL_MAGIC, 2, struct npu_buf_info)

#define NPU_IOCTL_MAX        3

#endif
