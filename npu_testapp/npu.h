#ifndef NPU_H
#define NPU_H
#include <linux/ioctl.h>

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

#define PCI_DMA_NPU_CMD_BUF_ADDRESS     0x410000000
#define PCI_DMA_NPU_WEIGHT_BUF_ADDRESS  0x412000000
#define PCI_DMA_NPU_WORK_BUF_ADDRESS    0x420000000
#define PCI_DMA_NPU_INPUT_BUF_ADDRESS   0x416000000
#define PCI_DMA_NPU_OUTPUT_BUF_ADDRESS  0x418000000

#define POST_NMS	1
#define POST_CLASS	2
#define POST_YOLOV2	3


#define COLOR_CONV_YUV 0
#define COLOR_CONV_RGB 1

#define NPU_IOCTL_MAGIC      'k'
 
#define NPU_IOCTL_BUFFER_CREATE		_IOW(NPU_IOCTL_MAGIC, 0, int)
#define NPU_IOCTL_NETWORK_CREATE  	_IOW(NPU_IOCTL_MAGIC, 1, struct npu_net_req)
#define NPU_IOCTL_RUN_INFERENCE		_IOW(NPU_IOCTL_MAGIC, 2, struct npu_buf_info)
#define NPU_IOCTL_SET_COLOR_CONV		_IOW(NPU_IOCTL_MAGIC, 6, int)
#define NPU_IOCTL_MAX        10

#endif
