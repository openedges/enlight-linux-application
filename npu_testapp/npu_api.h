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
/**
 *  Openedges Enlight Linux Application APIs
*/

int send_dma(char *devname, uint64_t addr, uint64_t size,
    uint64_t offset, uint64_t count, char *infname,
	char *ofname);

int read_dma(char *devname, uint64_t addr, uint64_t size,
	uint64_t offset, uint64_t count, char *ofname,
    char *buffer);

void run_post_process(void *inst, void *output_base);

int get_file_size_path(char *path);
int get_file_size(FILE *fp);

int YOLOV3_verify(char *outbuf);

int create_network(int fd, char *cmd, char *wei);
int create_buffer(int fd, int size);
int read_output_tensor(int out_fd, char *buffer, int size);

int run_inference(int net, int in_fd, int out_fd, char *input_file);
int set_color_conv(int net, int rgb_mode);

#endif
