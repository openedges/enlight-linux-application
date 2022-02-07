/**
 *  Openedges Enlight NPU Testapplication
 *
 *  Testapplication main function.
*/
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
#include "post_process.h"
#include "yolov4_waymo100k_3c_640_TDN_BNH_MIN32CH_quantized_network.h"
//#include "ssdlite300_quantized_network.h"
//#include "pytorch_mobilenet_v2_quantized_network.h"
//#include "yolov3_relu6_voc_quantized_network.h"

#define DEFAULT_CLASS	1000

#if 0
#define INPUT_FILE		"../example/pytorch_mobilenet_v2_quantized/sample/input.ia.bin"
#define CMD_FILE		"../example/pytorch_mobilenet_v2_quantized/network/npu_cmd.bin"
#define WEIGHT_FILE		"../example/pytorch_mobilenet_v2_quantized/network/quantized_network.bin"
#endif

#if 0 
#define INPUT_FILE		"../example/yolov3-relu6-voc_quantized/sample/input.ia.bin"
#define CMD_FILE		"../example/yolov3-relu6-voc_quantized/network/npu_cmd.bin"
#define WEIGHT_FILE		"../example/yolov3-relu6-voc_quantized/network/quantized_network.bin"
#endif

#if 0 
#define INPUT_FILE		"../example/ssdlite300_quantized/sample/input.ia.bin"
#define CMD_FILE		"../example/ssdlite300_quantized/network/npu_cmd.bin"
#define WEIGHT_FILE		"../example/ssdlite300_quantized/network/quantized_network.bin"
#endif

#if 1 
#define INPUT_FILE		"../example/yolov4-waymo100k_3c_640_TDN_BNH_MIN32CH_quantized/sample/input.ia.bin"
#define CMD_FILE		"../example/yolov4-waymo100k_3c_640_TDN_BNH_MIN32CH_quantized/network/npu_cmd.bin"
#define WEIGHT_FILE		"../example/yolov4-waymo100k_3c_640_TDN_BNH_MIN32CH_quantized/network/quantized_network.bin"
#endif

#if (MULTI)
void *network_thread(void *data);
void usage_multi_network(void);
extern char m_inputfile[128];
extern char m_cmdfile[128];
extern char m_weightfile[128];
extern int m_verify;
#endif

void print_usage(void)
{
	printf(" usage : options\n"
		    " -i : input image                , default : %s \n"
		    " -c : Network Command file       , default : %s \n"
		    " -w : Network Weight/bias file   , default : %s \n"
            " -v : Verify                     , default : none \n"
            "\t0: none,    1 : mobilenet, 2: ssdlite300\n"
            "\t3: YoloV3   4 : YoloV4\n"
		, INPUT_FILE
		, CMD_FILE
		, WEIGHT_FILE
	);
#if (MULTI)
    usage_multi_network();
#endif

}

int main(int argc, char **argv)
{
	int fd, in_fd, out_fd, net;
	int opt;
    int size, ret;
    int post_type;
    int verify = VERIFY_NONE;
	char *test_buf = NULL; // for post process work buffer
	char inputfile[128] = INPUT_FILE;
	char cmdfile[128] = CMD_FILE;
	char weightfile[128] = WEIGHT_FILE;
    int status;
    enlight_objs_t *d_objs = malloc(sizeof(enlight_objs_t));

	pthread_t pthread;
    pthread_mutex_init(&mutex, NULL);
    //enlight_network_t *net_ins = &pytorch_mobilenet_v2_quantized_network_instance;
    //enlight_network_t *net_ins = &ssdlite300_quantized_network_instance;
    //enlight_network_t *net_ins = &yolov3_relu6_voc_quantized_network_instance;
    enlight_network_t *net_ins = &yolov4_waymo100k_3c_640_TDN_BNH_MIN32CH_quantized_network_instance;

    while(-1 != (opt = getopt(argc, argv, "i:c:w:v:m:s:b:p:h"))) {
		switch (opt) {
        case 'i':   strcpy(inputfile, optarg);      	break;
		case 'c':   strcpy(cmdfile, optarg);        	break;
		case 'w':   strcpy(weightfile, optarg);     	break;
		case 'v':   verify = atoi(optarg);             	break;
#if (MULTI)
        case 'm':   strcpy(m_inputfile, optarg);      	break;
		case 's':   strcpy(m_cmdfile, optarg);        	break;
		case 'b':   strcpy(m_weightfile, optarg);     	break;
		case 'p':   m_verify = atoi(optarg);             	break;
#endif
		case 'h':   print_usage(); exit(0);		break;

		} 
	}	
	printf("NPU Test Application\n");

    fd = open("/dev/npu", O_RDWR);
	if (fd < 0) {
		printf(" open fail NPU Driver\n");
		exit(0);
	}

    mlx_kernel_load(fd, "./mlx_kernel.bin");

#if (MULTI)
    pthread_create(&pthread, NULL, network_thread, (void *)fd);
#endif

    pthread_mutex_lock(&mutex);

    printf("Create Network : %s \n", enlight_get_name(net_ins));
    net = create_network(fd, cmdfile, weightfile,
                    PCI_DMA_NPU_CMD_BUF_ADDRESS, PCI_DMA_NPU_WEIGHT_BUF_ADDRESS);
    if (net < 0) 
    {
        printf(" Create NPU Network fail!\n");
        goto net_fail;
    }
    pthread_mutex_unlock(&mutex);

    size = enlight_get_buf_size(net_ins, ENLIGHT_BUF_INPUT);
	in_fd = create_buffer(fd, size, PCI_DMA_NPU_INPUT_BUF_ADDRESS);
    if (in_fd < 0) 
    {
        printf(" Input Buffer Create fail!\n");
        goto infd_fail;
    }

    size = enlight_get_buf_size(net_ins, ENLIGHT_BUF_OUTPUT);
    out_fd = create_buffer(fd, size, PCI_DMA_NPU_OUTPUT_BUF_ADDRESS);
    if (out_fd < 0) 
    {
        printf(" Output Buffer Create fail!\n");
        goto outfd_fail;
    }
    int i;

    test_buf = (char *) malloc(size * sizeof(char));

    for(i=0; i< 1; i++) {

        pthread_mutex_lock(&mutex);
        ret = run_inference(net, in_fd, out_fd, inputfile, PCI_DMA_NPU_INPUT_BUF_ADDRESS );
        if (ret) 
        {
            printf("Network Inference fail!\n");
            goto out;
        }
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&mutex);
        size = enlight_get_buf_size(net_ins, ENLIGHT_BUF_OUTPUT);
        read_output_tensor(out_fd, test_buf, size, PCI_DMA_NPU_OUTPUT_BUF_ADDRESS);
    
        pthread_mutex_unlock(&mutex);

    	post_type = enlight_get_post_type(net_ins);
        if(post_type == ENLIGHT_POST_DETECTOR ) {
             ret =  run_post_process(net_ins, test_buf, d_objs);
             FILE *file = fopen("./pred.txt", "w");
            for (i = 0; i < d_objs->cnt ; i++) {
            enlight_obj_t *obj = &d_objs->obj[i];
            fprintf(file,
                "[%3d, %3d, %3d, %3d, %3d,% 3d,% 3d,% 3d]\n",
                obj->x_min, obj->x_max,
                obj->y_min, obj->y_max, 
                obj->cls,obj->score,
                obj->img_w, obj->img_h);
            }
            fclose(file);

        } else if (post_type == ENLIGHT_POST_CLASSIFIER) {
             ret =  run_post_process(net_ins, test_buf, NULL);
        }

        switch(verify) {
        case VERIFY_MOBILE:
            ret = mobilenet_verify(test_buf, net_ins);
            break;
        case VERIFY_SSDLITE:
            ret = ssdlite_verify(test_buf, net_ins);
            break;
        case VERIFY_YOLOV3:
            ret = yolov3_verify(test_buf, net_ins);
            break;
        case VERIFY_YOLOV4:
            ret = yolov4_verify(test_buf, net_ins);
            break;
        }
       
        if(verify) { 
            if(!ret)
                printf("Verification OK\n");
            else
                printf("Verification Fail\n");
        }
   }

#if (MULTI)
    pthread_join(pthread, (void **)&status);
#endif

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
