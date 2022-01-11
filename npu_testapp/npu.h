#ifndef NPU_H
#define NPU_H
#include <linux/ioctl.h>
#include "network/network_buf.h"

struct npu_buf_info {
	int in_fd;
	int out_fd;
};

struct npu_net_req {
	int cmd_size;
	int wei_size;
	char *cmd_data;
	char *wei_data;
};

#define POST_SSD	1
#define POST_CLASS	2
#define POST_YOLOV2	3
#define POST_EFFDET	4

#define NPU_IOCTL_MAGIC      'k'
 
#define NPU_IOCTL_BUFFER_CREATE		_IOW(NPU_IOCTL_MAGIC, 0, int)
#define NPU_IOCTL_NETWORK_CREATE  	_IOW(NPU_IOCTL_MAGIC, 1, struct npu_net_req)
#define NPU_IOCTL_RUN_INFERENCE		_IOW(NPU_IOCTL_MAGIC, 2, struct npu_buf_info)
#define NPU_IOCTL_WORK_BUFFER_CREATE	_IOW(NPU_IOCTL_MAGIC, 3, int)
#define NPU_IOCTL_GET_WORK_BUFFER_SIZE	_IO(NPU_IOCTL_MAGIC, 4)
#define NPU_IOCTL_STOP_NPU		_IO(NPU_IOCTL_MAGIC, 5)

#define NPU_IOCTL_MAX        10

#endif
