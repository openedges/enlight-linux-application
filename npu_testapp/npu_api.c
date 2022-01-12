#include "npu.h"
#include "npu_api.h"


int get_file_size_path(char *path)
{
	FILE *fp = fopen(path, "ro");
    int file_size;

    fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

    fclose(fp);
    return file_size;
}

int get_file_size(FILE *fp)
{
	int file_size;

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	return file_size;
}

int YOLOV3_verify(char *outbuf)
 {
	FILE *ref0 = fopen("../example/yolo3/sample/Conv2d_74.oa.bin", "ro");
	FILE *ref1 = fopen("../example/yolo3/sample/Conv2d_66.oa.bin", "ro");
    FILE *ref2 = fopen("../example/yolo3/sample/Conv2d_58.oa.bin", "ro");

	char *ref_buf;
	char *ref_buf1;
	char *ref_buf2;
	int input_size, ret;

	input_size = get_file_size(ref0);

	ref_buf = (char *) malloc(input_size * sizeof(char));
	ret =  fread(ref_buf, 1, input_size, ref0);
	if (ret < 0)
		printf(" Read fail yolov2 Golden data\n");


	ret = memcmp(ref_buf, outbuf, input_size);
	if(!ret)
		printf("Compare Success!\n");
	else
		printf("Compare Fail!\n");

    input_size = get_file_size(ref1);

	ref_buf1 = (char *) malloc(input_size * sizeof(char));
	ret =  fread(ref_buf1, 1, input_size, ref1);
	if (ret < 0)
		printf(" Read fail yolov2 Golden data\n");

	ret = memcmp(ref_buf1, outbuf+0x3f600, 0x1000);
	if(!ret)
		printf("Compare Success!\n");
	else
		printf("Compare Fail!\n");

    printf("%x %x  %x\n", *ref_buf1 , *(outbuf), *(outbuf+0x3f600));
    input_size = get_file_size(ref2);

	ref_buf2 = (char *) malloc(input_size * sizeof(char));
	ret =  fread(ref_buf2, 1, input_size, ref2);
	if (ret < 0)
		printf(" Read fail yolov2 Golden data\n");


	ret = memcmp(ref_buf2, outbuf+0x4f380, 0x1000);
	if(!ret)
		printf("Compare Success!\n");
	else
		printf("Compare Fail!\n");
  
	free(ref_buf);
	free(ref_buf1);
	free(ref_buf2);
	fclose(ref0);
	fclose(ref1);
	fclose(ref2);

	return ret;
}

int create_network(int fd, char *cmd, char *wei)
{
    int ret, net;
    int cmd_size =  get_file_size_path(cmd);
    int weight_size = get_file_size_path(wei);

	struct npu_net_req *net_req;
    ret = send_dma("/dev/xdma0_h2c_0", PCI_DMA_NPU_CMD_BUF_ADDRESS, cmd_size, 0, 1, cmd, NULL);
    if(ret) 
    {        
            printf("NPU CMD file open Error!\n");
            return -1;
    }
    
    ret = send_dma("/dev/xdma0_h2c_0", PCI_DMA_NPU_WEIGHT_BUF_ADDRESS, weight_size, 0, 1, wei, NULL);
     if(ret) 
    {        
            printf("NPU Weight/Bias file open Error!\n");
            return -1;
    }
    net_req = malloc(sizeof(net_req));
	net_req->cmd_size = cmd_size;
	net_req->wei_size = weight_size;

    net = ioctl(fd, NPU_IOCTL_NETWORK_CREATE, net_req);

    free(net_req);
    return net;

}

int create_buffer(int fd, int size)
{
    return ioctl(fd, NPU_IOCTL_BUFFER_CREATE, size);
}

int read_output_tensor(int out_fd, char *buffer, int size)
{
   int ret;

   ret = read_dma("/dev/xdma0_c2h_0", PCI_DMA_NPU_OUTPUT_BUF_ADDRESS, size, 0, 1, "dump.bin", buffer);

   return ret;
}

int run_inference(int net, int in_fd, int out_fd, char *input_file)
{
    int ret;
    int size = get_file_size_path(input_file);
	struct npu_buf_info inf_buf;

    ret = send_dma("/dev/xdma0_h2c_0", PCI_DMA_NPU_INPUT_BUF_ADDRESS, size, 0, 1, input_file, NULL);
    if(ret) 
    {        
            printf("Input file load Error!\n");
            return -1;
    }

    inf_buf.in_fd = in_fd;	
	inf_buf.out_fd = out_fd;	

	ret = ioctl(net, NPU_IOCTL_RUN_INFERENCE, &inf_buf);
	if(ret) {
		printf("INFERENCE Fail %d\n", ret);
	}

    return ret;
}

int set_color_conv(int net, int rgb_mode)
{
    int ret;
	ret = ioctl(net, NPU_IOCTL_SET_COLOR_CONV, rgb_mode);
    return ret;
}
