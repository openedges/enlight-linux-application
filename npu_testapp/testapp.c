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
#include "enx/include/enx_vsys.h"
#include "enx/include/enx_uyv.h"

#include <pthread.h>
#include <poll.h>

#define INPUT_FILE		"/App/test/input.ia.bin"
#define CMD_FILE		"/App/test/npu_cmd.bin"
#define WEIGHT_FILE		"/App/test/quantized_network.bin"
#define SCORE_FILE		"/App/test/Concat_270.oa.bin"
#define LOCATION_FILE		"/App/test/Concat_269.oa.bin"
#define CLASS_GOLDEN_FILE	"/App/test/Gemm_151.oa.bin"
#define YOLOV2_GOLDEN_FILE	"/App/test/yolov2/Conv2d_22.oa.bin"
#define DEFAULT_CLASS	1000

#define PAGE_SHIFT 12
#define ACT_BUF_SIZE	15 * 1024 * 1024
#define NSEC_PER_MSEC	1000000L

UYV_PARAMS_S gUyvParams;

void print_usage(void)
{
	printf(" usage : options\n"
		    " -i : input image                , default : %s \n"
		    " -c : Network Command file       , default : %s \n"
		    " -w : Network Weight/bias file   , default : %s \n"
		    " -l : Concat_0 golden data file  , default : %s \n"
		    " -s : Concat_1 golden data file  , default : %s \n"
		    " -g : Classifer golden data file , default : %s \n"
		    " -p : post process \n"
	   	    " \t1 : NMS , 2 : Classifier , default : NMS \n"	 
		    " -n : class num                  , default : %d \n"
		    " -v : verification out tensor & golden data     \n"
		, INPUT_FILE
		, CMD_FILE
		, WEIGHT_FILE
		, LOCATION_FILE
		, SCORE_FILE
		, CLASS_GOLDEN_FILE
		, DEFAULT_CLASS
	);
}

void run_post_process(void *oact_base, void *work_base);
int classifier_run(u_int8_t *oact_base, int log2_oact_scl, int num_class);

int get_file_size(FILE *fp)
{
	int file_size;

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	return file_size;
}
int Classification_verify(char *ref_path, char *outbuf)
{
	FILE *ref = fopen(ref_path, "ro");
	char *ref_buf;
	int input_size, ret;

	input_size = get_file_size(ref);
	
	ref_buf = (char *) malloc(input_size * sizeof(char));
	ret =  fread(ref_buf, 1, input_size, ref);
	if (ret < 0)
		printf(" Read fail Score Golden data\n");
	printf("Ref Class %d\n", classifier_run((u_int8_t *)ref_buf, 1000,1000));
	printf("R Class %d\n", classifier_run((u_int8_t *)outbuf, 1000,1000));
	ret = memcmp(ref_buf, outbuf, input_size);
	if(!ret)
		printf("Output file Compare OK!\n");
	else
		printf("Output file Compare diff!\n");

	return ret;
}

int NMS_verify(char *outbuf, char *location, char *score)
{
	FILE *sco = fopen(score, "ro");
	FILE *loc = fopen(location, "ro");
	char *sco_buf, *loc_buf;
	int input_size, ret;

	input_size = get_file_size(sco);

	sco_buf = (char *) malloc(input_size * sizeof(char));
	ret =  fread(sco_buf, 1, input_size, sco);
	if (ret < 0)
		printf(" Read fail Score Golden data\n");

	input_size = get_file_size(loc);
	loc_buf = (char *) malloc(input_size * sizeof(char));


	ret =  fread(loc_buf, 1, input_size, loc);
	if (ret < 0)
		printf(" Read fail location Golden data\n");
	
	ret = memcmp(sco_buf, outbuf, input_size);
	if(!ret)
		printf("Score Compare Success!\n");
	else
		printf("Score Compare Fail!\n");

	
	ret = memcmp(loc_buf, outbuf + 0x18700, input_size);
	if(!ret)
		printf("LOCATION Compare Success!\n");
	else
		printf("LOCATION Compare Fail!\n");

	free(sco_buf);
	free(loc_buf);
	fclose(sco);
	fclose(loc);

	return ret;
}

int YOLOV2_verify(char *outbuf, char *refpath)
{
	FILE *ref = fopen(refpath, "ro");
	FILE *save = fopen("/usr/bin/npu/yolov2/save.bin", "wr");
	char *ref_buf;
	int input_size, ret;

	input_size = get_file_size(ref);

	ref_buf = (char *) malloc(input_size * sizeof(char));
	ret =  fread(ref_buf, 1, input_size, ref);
	if (ret < 0)
		printf(" Read fail yolov2 Golden data\n");


	ret = memcmp(ref_buf, outbuf, input_size);
	if(!ret)
		printf("Compare Success!\n");
	else
		printf("Compare Fail!\n");
	
	ret =  fwrite(outbuf, 1, input_size, save);
	free(ref_buf);
	fclose(ref);
	fclose(save);

	return ret;
}

int main(int argc, char **argv)
{
	int fd, in_fd, out_fd, work_fd, net;
	int opt;
	int verify = 0;
	char *cn1_buf = NULL, *cn0_buf = NULL;
	char *test_buf = NULL; // for post process work buffer

	char *in_buf = NULL, *out_buf = NULL;
	
	int input_size, output_size =0x30000, cmp_size = 0x17700;
	char inputfile[128] = INPUT_FILE;
	char cmdfile[128] = CMD_FILE;
	char weightfile[128] = WEIGHT_FILE;
	char score_file[128] = SCORE_FILE;
	char location_file[128] = LOCATION_FILE;
	char classfier_file[128] = CLASS_GOLDEN_FILE;
	char yolov2_file[128] = YOLOV2_GOLDEN_FILE;
	int post = POST_NMS, class = DEFAULT_CLASS;

	FILE *cmd_fp = NULL;
	FILE *wei_fp = NULL;
	FILE *in_fp = NULL;
	FILE *cn1_fp = NULL;
	FILE *cn0_fp = NULL;

	FILE *m_cmd_fp = NULL;
	FILE *m_wei_fp = NULL;
	FILE *m_in_fp = NULL;

	struct npu_buf_info inf_buf;
	struct npu_net_req *net_req;
	struct npu_net_req *m_net_req;
	int ret;
	long ioret;
	int status;
	int loop = 10;
	while(-1 != (opt = getopt(argc, argv, "i:c:w:l:s:vhp:n:x:r:"))) {
		switch (opt) {
		case 'i':   strcpy(inputfile, optarg);      	break;
		case 'c':   strcpy(cmdfile, optarg);        	break;
		case 'w':   strcpy(weightfile, optarg);     	break;
		case 'l':   strcpy(location_file, optarg);  	break;
		case 's':   strcpy(score_file, optarg); 	break;
		case 'v':   verify = 1;                 	break;
		case 'p':   post = atoi(optarg);  		break;
		case 'n':   class = atoi(optarg);		break;
		case 'g':   strcpy(classfier_file, optarg);	break;
		case 'h':   print_usage(); exit(0);		break;
		case 'r':   loop = atoi(optarg);		break;
		} 
	}	
	printf("NPU Test Application\n");

/*
 * Open device 
 */
	if(ENX_VSYS_Init() != 0){
		printf("ENX_VSYS_Init failed\n");
		exit(0);
	}


	if(ENX_UYV_CAPTURE_Init(&gUyvParams) != 0){
	    ENX_VSYS_Exit();
		printf("ENX_UYV_CAPTURE_Init failed\n");
		exit(0);
	}

	system("devmem 0x45100008 32 0x00bf0200");

	fd = open("/dev/npu", O_RDWR);
	if (fd < 0) {
		printf(" open fail NPU Driver\n");
		exit(0);
	}
	
	in_fp = fopen(inputfile, "ro");
	if (!in_fp) {
		printf("input file open Error!\n");
		goto exit;
	}

	cmd_fp = fopen(cmdfile, "ro");
	if (!cmd_fp) {
		printf("cmd file open Error!\n");
		goto exit;
	}

	wei_fp = fopen(weightfile, "ro");
	if (!wei_fp) {
		printf("weight file open Error!\n");
		goto exit;
	}

/*
 * Network Create
 */
	//net_req = malloc(sizeof(struct npu_net_req));
	net_req = malloc(sizeof(net_req));

	#if 1	
	net_req->cmd_size = get_file_size(cmd_fp);
	net_req->wei_size = get_file_size(wei_fp);
	#else
	net_req->cmd_size = npu_cmd_code_size;
	net_req->wei_size = quant_network_size;
	#endif
	net_req->cmd_data = malloc(net_req->cmd_size);
	net_req->wei_data = malloc(net_req->wei_size);

	ret = fread(net_req->cmd_data , 1, net_req->cmd_size, cmd_fp);
	if (ret < 0) {
		printf("Read Command file fail\n");
		goto exit;
	}

	ret = fread(net_req->wei_data , 1, net_req->wei_size, wei_fp);
	if (ret < 0) {
		printf("Read Wegiht/bias file fail\n");
		goto exit;
	}
	sync();

	net = ioctl(fd, NPU_IOCTL_NETWORK_CREATE, net_req);

	free(net_req->cmd_data);
	free(net_req->wei_data);
	free(net_req);

	in_fd = ioctl(fd, NPU_IOCTL_BUFFER_CREATE, input_buf_size);
	out_fd = ioctl(fd, NPU_IOCTL_BUFFER_CREATE, output_buf_size );

	input_size = get_file_size(in_fp);
	in_buf = (char*)mmap((void*)0, input_buf_size, PROT_READ|PROT_WRITE, MAP_SHARED, in_fd, 0 << PAGE_SHIFT);
	
	msync(in_buf, input_buf_size, MS_SYNC);
	
	ret =  fread(in_buf, 1, input_size, in_fp);
	if (ret < 0) {
		printf("Read input file fail\n");
		goto exit;
	}

	system("echo 1 > /proc/sys/vm/drop_caches");
	out_buf = (char *)mmap((void*)0, output_buf_size, PROT_READ|PROT_WRITE, MAP_SHARED, out_fd, 0 << PAGE_SHIFT);
	msync(out_buf, output_buf_size, MS_SYNC);

	inf_buf.in_fd = in_fd;	
	inf_buf.out_fd = out_fd;	

	ret = ioctl(net, NPU_IOCTL_RUN_INFERENCE, &inf_buf);
	if(ret) {
		printf("INFERENCE Fail %d\n", ret);
	}

	struct pollfd fds;
	
	fds.fd = net;
	fds.events = POLLIN | POLLOUT | POLLERR;
	ret = poll((struct pollfd *)&fds, 1, 100 * 1000);

	if (post == POST_NMS || post == POST_YOLOV2)
		run_post_process((void *)out_buf,NULL);
	else if (post == POST_CLASS) 
		printf("\n\n\nClass : %d\n\n\n",classifier_run((u_int8_t *)out_buf, 1000, class));

	if (verify) { 

		switch (post) {
		case POST_NMS:
			NMS_verify(out_buf, location_file, score_file);
		break;
		case POST_CLASS :
			Classification_verify(classfier_file, out_buf);
		break;
		case POST_YOLOV2: {
			FILE *t = fopen(yolov2_file, "ro");
			char *tbuf;
			int ssize =  get_file_size(t);
			tbuf = malloc(ssize);
			fread(tbuf, 1, ssize, t);	
			
			run_post_process((void *)tbuf,test_buf);
			YOLOV2_verify(out_buf, yolov2_file);
			fclose(t);
			free(tbuf);
		}
		break;

		}
	}

exit:

	munmap(in_buf, input_buf_size);
	munmap(out_buf, output_buf_size);
	printf("TEST Done!\n");
	
	fclose(cmd_fp);
	fclose(wei_fp);
	fclose(in_fp);
	
	close(net);
	close(in_fd);	
	close(out_fd);	
	close(fd);

	return 0;
}
