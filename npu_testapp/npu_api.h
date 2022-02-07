
/**
 *  Openedges Enlight NPU Testapplication
 *
 *  Npu API.
*/
#ifndef __ENLIGHT_APPLICATION_APIs__
#define __ENLIGHT_APPLICATION_APIs__

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include "enlight_data_type.h"
/**
 *  Openedges Enlight Linux Application APIs
*/

int send_dma(char *devname, uint64_t addr, uint64_t size,
    uint64_t offset, uint64_t count, char *infname,
	char *ofname);

int read_dma(char *devname, uint64_t addr, uint64_t size,
	uint64_t offset, uint64_t count, char *ofname,
    char *buffer);

int get_file_size_path(char *path);
int get_file_size(FILE *fp);

int YOLOV4_verify(char *outbuf, enlight_network_t *network_instance);
int yolov3_verify(char *outbuf, enlight_network_t *network_instance);
int ssdlite_verify(char *outbuf, enlight_network_t *network_instance);
int mobilenet_verify(char *outbuf, enlight_network_t *network_instance);

int create_network(int fd, char *cmd, char *wei, uint64_t cmd_addr, uint64_t wei_addr);
int create_buffer(int fd, int size, uint64_t addr);
//int read_output_tensor(int out_fd, char *buffer, int size);
int read_output_tensor(int out_fd, char *buffer, int size, uint64_t addr);

//int run_inference(int net, int in_fd, int out_fd, char *input_file);
int run_inference(int net, int in_fd, int out_fd, char *input_file, uint64_t addr);
int set_color_conv(int net, int rgb_mode);

int mlx_kernel_load(int fd, char *path);

#endif
