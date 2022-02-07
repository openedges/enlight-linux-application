/**
 *  Openedges Enlight NPU Testapplication
 *
 *  Npu dMulti network support.
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
#if (MULTI)
#include "ssdlite300_quantized_network.h"
#include "multi_post_process.h"

#define MULTI_INPUT_FILE	"../example/ssdlite300_quantized/sample/input.ia.bin"
#define MULTI_CMD_FILE		"../example/ssdlite300_quantized/network/npu_cmd.bin"
#define MULTI_WEIGHT_FILE	"../example/ssdlite300_quantized/network/quantized_network.bin"

void usage_multi_network(void)
{
    printf(
        " -m : 2nd Network input image      , default : %s \n"
	    " -s : 2nd Network Command file     , default : %s \n"
	    " -b : 2nd Network Weight/bias file , default : %s \n"
        " -p : Verify                     , default : none \n"
            "\t0: none,    1 : mobilenet, 2: ssdlite300\n"
            "\t3: YoloV3   4 : YoloV4\n"
	    , MULTI_INPUT_FILE
	    , MULTI_CMD_FILE
	    , MULTI_WEIGHT_FILE
    );
}

char m_inputfile[128] = MULTI_INPUT_FILE;
char m_cmdfile[128] = MULTI_CMD_FILE;
char m_weightfile[128] = MULTI_WEIGHT_FILE;
int m_verify = VERIFY_NONE;

void *network_thread(void *data)
{
    int fd = data;
    int net;
    int size, ret, post_type;
    int in_fd, out_fd;
	char *test_buf = NULL; // for post process work buffer
    enlight_network_t *net_ins =  &ssdlite300_quantized_network_instance;
 
    enlight_objs_t *d_objs = malloc(sizeof(enlight_objs_t));

    pthread_mutex_lock(&mutex);
    printf("Create Network : %s \n", enlight_get_name(net_ins));
    net = create_network(fd, m_cmdfile, m_weightfile,
                    PCI_DMA_NPU_CMD2_BUF_ADDRESS, PCI_DMA_NPU_WEIGHT2_BUF_ADDRESS);
    if (net < 0) 
    {
        printf(" Create NPU Network fail!\n");
        goto net_fail;
    }
    pthread_mutex_unlock(&mutex);

    size = enlight_get_buf_size(net_ins, ENLIGHT_BUF_INPUT);
	in_fd = create_buffer(fd, size, PCI_DMA_NPU_INPUT2_BUF_ADDRESS);
    if (in_fd < 0) 
    {
        printf(" Input Buffer Create fail!\n");
        goto infd_fail;
    }

    size = enlight_get_buf_size(net_ins, ENLIGHT_BUF_OUTPUT);
    out_fd = create_buffer(fd, size, PCI_DMA_NPU_OUTPUT2_BUF_ADDRESS);
    if (out_fd < 0) 
    {
        printf(" Output Buffer Create fail!\n");
        goto outfd_fail;
    }

    test_buf = (char *) malloc(size * sizeof(char));

    int i; 
    for(i=0; i< 1; i++) {
        pthread_mutex_lock(&mutex);
        ret = run_inference(net, in_fd, out_fd, m_inputfile, PCI_DMA_NPU_INPUT2_BUF_ADDRESS);
        if (ret) 
        {
            printf("Network Inference fail!\n");
            goto out;
        }

        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&mutex);
        size = enlight_get_buf_size(net_ins, ENLIGHT_BUF_OUTPUT);
        read_output_tensor(out_fd, test_buf, size, PCI_DMA_NPU_OUTPUT2_BUF_ADDRESS );
    
        pthread_mutex_unlock(&mutex);

    	post_type = enlight_get_post_type(net_ins);
        if(post_type == ENLIGHT_POST_DETECTOR ) {
             ret = multi_post_process(net_ins, test_buf, d_objs);
        } else if (post_type == ENLIGHT_POST_CLASSIFIER) {
             ret = multi_post_process(net_ins, test_buf, NULL);
        }

        switch(m_verify) {
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

        if(m_verify) {
            if(!ret)
                printf("Verification OK\n");
            else
                printf("Verification Fail\n");
        }
   }

    free(test_buf);
out:
    close(out_fd);
outfd_fail:
    close(in_fd);
infd_fail :
    close(net);
net_fail :
    close(fd);
}
#endif
