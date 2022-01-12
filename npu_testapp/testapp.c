#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include "npu.h"
#include <sys/types.h>
#include "npu_api.h"
#include "enlight_data_type.h"
#include "enlight_network.h"
#include "network.h"

#define INPUT_FILE		"../example/yolo3/sample/input.ia.bin"
#define CMD_FILE		"../example/yolo3/network/npu_cmd.bin"
#define WEIGHT_FILE		"../example/yolo3/network/quantized_network.bin"
#define DEFAULT_CLASS	1000

void print_usage(void)
{
	printf(" usage : options\n"
		    " -i : input image                , default : %s \n"
		    " -c : Network Command file       , default : %s \n"
		    " -w : Network Weight/bias file   , default : %s \n"
		, INPUT_FILE
		, CMD_FILE
		, WEIGHT_FILE
	);
}

int main(int argc, char **argv)
{
	int fd, in_fd, out_fd, net;
	int opt;
	int verify = 0;
    int size;
	char *test_buf = NULL; // for post process work buffer
	char inputfile[128] = INPUT_FILE;
	char cmdfile[128] = CMD_FILE;
	char weightfile[128] = WEIGHT_FILE;
	int ret;
	
    while(-1 != (opt = getopt(argc, argv, "i:c:w:h"))) {
		switch (opt) {
		case 'i':   strcpy(inputfile, optarg);      	break;
		case 'c':   strcpy(cmdfile, optarg);        	break;
		case 'w':   strcpy(weightfile, optarg);     	break;
		case 'h':   print_usage(); exit(0);		break;
		} 
	}	
	printf("NPU Test Application\n");

	fd = open("/dev/npu", O_RDWR);
	if (fd < 0) {
		printf(" open fail NPU Driver\n");
		exit(0);
	}

    net = create_network(fd, cmdfile, weightfile);
    if (net < 0) 
    {
        printf(" Create NPU Network fail!\n");
        goto net_fail;
    }

    size = enlight_get_input_buf_size(&network_instance);
	in_fd = create_buffer(fd, size);
    if (in_fd < 0) 
    {
        printf(" Input Buffer Create fail!\n");
        goto infd_fail;
    }

    size = enlight_get_output_buf_size(&network_instance);
    out_fd = create_buffer(fd, size);
    if (out_fd < 0) 
    {
        printf(" Output Buffer Create fail!\n");
        goto outfd_fail;
    }

    ret = run_inference(net, in_fd, out_fd, inputfile);
    if (ret) 
    {
        printf("Network Inference fail!\n");
        goto out;
    }

    test_buf = (char *) malloc(16 * 1024  * 1024 * sizeof(char));

    size = enlight_get_output_buf_size(&network_instance);
    read_output_tensor(out_fd, test_buf, size);
    
    run_post_process(&network_instance, test_buf);

    free(test_buf);
out:
    close(out_fd);
outfd_fail:
    close(in_fd);
infd_fail :
    close(net);
net_fail :
    close(fd);
	return 0;
}
