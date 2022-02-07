
/**
 *  Openedges Enlight NPU Testapplication
 *
 *  Npu APIs.
*/
#include "npu.h"
#include "npu_api.h"
#include <linux/poll.h>
#include "enlight_data_type.h"
#include "enlight_network.h"


int get_file_size_path(char *path)
{
	FILE *fp; 
    int file_size;


    if(access(path, F_OK)) {
            printf("file open fail \n");
            return -1;
    }

    fp = fopen(path, "ro");
             
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

int mobilenet_verify(char *outbuf, enlight_network_t *network_instance)
{
    FILE *ref = fopen("../example/pytorch_mobilenet_v2_quantized/sample/Gemm_151.oa.bin", "ro");
    enlight_act_tensor_t *tensor;
    char *ref_buf;
	int input_size, ret;
    int off, size;

    input_size = get_file_size(ref);

    tensor = enlight_get_output_tensor(network_instance, 0);
    size = enlight_get_tensor_size(tensor);
    off = enlight_get_tensor_offset(tensor);

    off = network_instance->output_tensors[0]->buf;
        
    ref_buf = (char *) malloc(input_size * sizeof(char));
	ret =  fread(ref_buf, 1, input_size, ref);
	if (ret < 0) {
	    printf(" Read fail Golden data\n");
    }

    ret = memcmp(ref_buf, outbuf+off, size);
	    
    free(ref_buf);
    fclose(ref);
    if(ret) 
	    printf("Compare Fail!\n");

    return ret;
}

int ssdlite_verify(char *outbuf, enlight_network_t *network_instance)
{
    FILE *ref[2];
	char *ref_buf;
    enlight_act_tensor_t *tensor;
	int input_size, ret, tensor_num;
    int off, size;
    int i;

    ref[0] = fopen("../example/ssdlite300_quantized/sample/Concat_269.oa.bin", "ro");
    ref[1] = fopen("../example/ssdlite300_quantized/sample/Concat_270.oa.bin", "ro");
   
    tensor_num =  enlight_get_output_tensor_num(network_instance);
    for (i = 0; i < tensor_num; i++) {

        tensor = enlight_get_output_tensor(network_instance, i);
        size = enlight_get_tensor_size(tensor);
    	input_size = get_file_size(ref[i]);
        off = enlight_get_tensor_offset(tensor);

	    ref_buf = (char *) malloc(input_size * sizeof(char));
	    ret =  fread(ref_buf, 1, input_size, ref[i]);
	    if (ret < 0) {
		    printf(" Read fail ssdlite Golden data\n");
        }

    	ret = memcmp(ref_buf, outbuf+off, size);
	    
        free(ref_buf);
        fclose(ref[i]);
        if(ret) {
		    printf("Compare Fail!\n");
            break;
        } 
    }

	return ret;
}

int yolov3_verify(char *outbuf, enlight_network_t *network_instance) 
{
    FILE *ref[6];
    char *ref_buf;
    enlight_act_tensor_t *tensor;
	int input_size, ret, tensor_num;
    int off, size;
    int i;

	ref[0] = fopen("../example/yolov3-relu6-voc_quantized/sample/Conv2d_75.oa.bin", "ro");
	ref[1] = fopen("../example/yolov3-relu6-voc_quantized/sample/Conv2d_76.oa.bin", "ro");
    ref[2] = fopen("../example/yolov3-relu6-voc_quantized/sample/Conv2d_77.oa.bin", "ro");
    ref[3] = fopen("../example/yolov3-relu6-voc_quantized/sample/Conv2d_78.oa.bin", "ro");
	ref[4] = fopen("../example/yolov3-relu6-voc_quantized/sample/Conv2d_79.oa.bin", "ro");
    ref[5] = fopen("../example/yolov3-relu6-voc_quantized/sample/Conv2d_80.oa.bin", "ro");

    tensor_num =  enlight_get_output_tensor_num(network_instance);
    for (i = 0; i < tensor_num; i++) {
    	tensor = enlight_get_output_tensor(network_instance, i);
        size = enlight_get_tensor_size(tensor);
    	input_size = get_file_size(ref[i]);
        off = enlight_get_tensor_offset(tensor);

	    ref_buf = (char *) malloc(input_size * sizeof(char));
	    ret =  fread(ref_buf, 1, input_size, ref[i]);
	    if (ret < 0) {
		    printf(" Read fail yolov3 Golden data\n");

        }

    	ret = memcmp(ref_buf, outbuf+off, size);
	    
        free(ref_buf);
        fclose(ref[i]);
        if(ret) {
		    printf("Compare Fail!\n");
            break;
        } 
    }

	return ret;
}

int yolov4_verify(char *outbuf, enlight_network_t *network_instance )
{
    FILE *ref[6];
	char *ref_buf;
    enlight_act_tensor_t *tensor;
	int input_size, ret;
    int off, size;
    int i;
    
    ref[0] = fopen("../example/yolov4-waymo100k_3c_640_TDN_BNH_MIN32CH_quantized/sample/Conv2d_192.oa.bin", "ro");
    ref[1] = fopen("../example/yolov4-waymo100k_3c_640_TDN_BNH_MIN32CH_quantized/sample/Conv2d_193.oa.bin", "ro");
    ref[2] = fopen("../example/yolov4-waymo100k_3c_640_TDN_BNH_MIN32CH_quantized/sample/Conv2d_194.oa.bin", "ro");
    ref[3] = fopen("../example/yolov4-waymo100k_3c_640_TDN_BNH_MIN32CH_quantized/sample/Conv2d_195.oa.bin", "ro");
    ref[4] = fopen("../example/yolov4-waymo100k_3c_640_TDN_BNH_MIN32CH_quantized/sample/Conv2d_196.oa.bin", "ro");
    ref[5] = fopen("../example/yolov4-waymo100k_3c_640_TDN_BNH_MIN32CH_quantized/sample/Conv2d_197.oa.bin", "ro");
    
    for (i = 0; i < 6;i++) {
        tensor = enlight_get_output_tensor(network_instance, i);
        size = enlight_get_tensor_size(tensor);
    	input_size = get_file_size(ref[i]);
        off = enlight_get_tensor_offset(tensor);
        
	    ref_buf = (char *) malloc(input_size * sizeof(char));
	    ret =  fread(ref_buf, 1, input_size, ref[i]);
	    if (ret < 0) {
		    printf(" Read fail yolov4 Golden data\n");

        }

    	ret = memcmp(ref_buf, outbuf+off, size);
	    
        free(ref_buf);
        fclose(ref[i]);
        if(ret) {
		    printf("Compare Fail!\n");
            break;
        } 
    }

	return ret;
}

int create_network(int fd, char *cmd, char *wei, 
                uint64_t cmd_addr, uint64_t wei_addr)
{
    int ret, net;
    int cmd_size =  get_file_size_path(cmd);
    int weight_size = get_file_size_path(wei);

	struct npu_net_req *net_req;
    //ret = send_dma("/dev/xdma0_h2c_0", PCI_DMA_NPU_CMD_BUF_ADDRESS, cmd_size, 0, 1, cmd, NULL);
    ret = send_dma("/dev/xdma0_h2c_0", cmd_addr, cmd_size, 0, 1, cmd, NULL);
    if(ret) 
    {        
            printf("NPU CMD file open Error!\n");
            return -1;
    }
    
    //ret = send_dma("/dev/xdma0_h2c_0", PCI_DMA_NPU_WEIGHT_BUF_ADDRESS, weight_size, 0, 1, wei, NULL);
    ret = send_dma("/dev/xdma0_h2c_0", wei_addr, weight_size, 0, 1, wei, NULL);
     if(ret) 
    {        
            printf("NPU Weight/Bias file open Error!\n");
            return -1;
    }
    net_req = malloc(sizeof(net_req));
	net_req->cmd_size = cmd_size;
	net_req->cmd_addr = (int)(cmd_addr >> 4);
	net_req->wei_size = weight_size;
	net_req->wei_addr = (int)(wei_addr >> 4);

    net = ioctl(fd, NPU_IOCTL_NETWORK_CREATE, net_req);

    free(net_req);
    return net;

}

int create_buffer(int fd, int size, uint64_t addr)
{
    struct npu_buf_req *buf_req;

    buf_req = malloc(sizeof(struct npu_buf_req));
	buf_req->size = size;
	buf_req->addr = (addr >> 4);

    return ioctl(fd, NPU_IOCTL_BUFFER_CREATE, buf_req);
}

int read_output_tensor(int out_fd, char *buffer, int size, uint64_t addr)
{
   int ret;

   //ret = read_dma("/dev/xdma0_c2h_0", addr, size, 0, 1, "dump.bin", buffer);
   ret = read_dma("/dev/xdma0_c2h_0", addr, size, 0, 1, NULL, buffer);

   return ret;
}

int run_inference(int net, int in_fd, int out_fd, char *input_file, uint64_t addr)
{
    int ret;
    int size = get_file_size_path(input_file);
	struct npu_buf_info inf_buf;

    ret = send_dma("/dev/xdma0_h2c_0", addr, size, 0, 1, input_file, NULL);
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

    struct pollfd fds;
    
    fds.fd = net;
    fds.events = POLLIN | POLLOUT | POLLERR;
    ret = poll((struct pollfd *)&fds, 1, 10 * 1000);

    if(ret == 1)
             ret = 0;
    return ret;
}

int set_color_conv(int net, int rgb_mode)
{
    int ret;
	ret = ioctl(net, NPU_IOCTL_SET_COLOR_CONV, rgb_mode);
    return ret;
}

int mlx_kernel_load(int fd, char *path)
{
    int ret;
    int size = get_file_size_path(path);

    if(size < 0)
        return -1;

    ret = send_dma("/dev/xdma0_h2c_0", PCI_DMA_NPU_MLX_KERNEL_ADDRESS, size, 0, 1, path, NULL);
    if(ret) 
    {        
            printf("mlx kernel load Error!\n");
            return -1;
    }   
	ret = ioctl(fd, NPU_IOCTL_LOAD_MLX_KERNEL, PCI_DMA_NPU_MLX_KERNEL_ADDRESS);
    return ret;
}

