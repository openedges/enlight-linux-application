/**
 *  Openedges Enlight NPU Testapplication
 *
 *  Npu definitions.
*/
  
#ifndef NPU_H
#define NPU_H
#include <linux/ioctl.h>
#include <stdint.h>
#include <pthread.h>

#define MULTI 1

pthread_mutex_t mutex;

struct npu_buf_info {
	int in_fd;
	int out_fd;
};

struct npu_net_req {
	int cmd_size;
	int wei_size;
    int cmd_addr;
    int wei_addr;
	char *cmd_data;
	char *wei_data;
};

struct npu_buf_req {
    int size;
    uint64_t addr;
};

typedef enum {
    VERIFY_NONE     = 0,
    VERIFY_MOBILE   = 1,
    VERIFY_SSDLITE  = 2,
    VERIFY_YOLOV3   = 3,
    VERIFY_YOLOV4   = 4,
} npu_verify_t;

#define PCI_DMA_NPU_CMD_BUF_ADDRESS     0x410000000
#define PCI_DMA_NPU_WEIGHT_BUF_ADDRESS  0x412000000
#define PCI_DMA_NPU_WORK_BUF_ADDRESS    0x440000000
#define PCI_DMA_NPU_INPUT_BUF_ADDRESS   0x416000000
#define PCI_DMA_NPU_OUTPUT_BUF_ADDRESS  0x418000000

#define PCI_DMA_NPU_CMD2_BUF_ADDRESS     0x430000000
#define PCI_DMA_NPU_WEIGHT2_BUF_ADDRESS  0x432000000
#define PCI_DMA_NPU_INPUT2_BUF_ADDRESS   0x436000000
#define PCI_DMA_NPU_OUTPUT2_BUF_ADDRESS  0x438000000

#define PCI_DMA_NPU_MLX_KERNEL_ADDRESS  0x480000000

#define COLOR_CONV_YUV 0
#define COLOR_CONV_RGB 1

#define NPU_IOCTL_MAGIC      'k'
 
#define NPU_IOCTL_BUFFER_CREATE		_IOW(NPU_IOCTL_MAGIC, 0, int)
#define NPU_IOCTL_NETWORK_CREATE  	_IOW(NPU_IOCTL_MAGIC, 1, struct npu_net_req)
#define NPU_IOCTL_RUN_INFERENCE		_IOW(NPU_IOCTL_MAGIC, 2, struct npu_buf_info)
#define NPU_IOCTL_LOAD_MLX_KERNEL	_IOW(NPU_IOCTL_MAGIC, 5, int)
#define NPU_IOCTL_SET_COLOR_CONV		_IOW(NPU_IOCTL_MAGIC, 6, int)
#define NPU_IOCTL_MAX        10

#endif
