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
#include "npu.h"
#include "network/network_buf.h"

#define INPUT_FILE		"/usr/bin/npu/input.ia.bin"
#define CMD_FILE		"/usr/bin/npu/npu_cmd.bin"
#define WEIGHT_FILE		"/usr/bin/npu/quantized_network.h.bin"
#define SCORE_FILE		"/usr/bin/npu/Concat_270.oa.bin"
#define LOCATION_FILE		"/usr/bin/npu/Concat_269.oa.bin"
#define CLASS_GOLDEN_FILE	"/usr/bin/npu/Gemm_151.oa.bin"
#define YOLOV2_GOLDEN_FILE	"/usr/bin/npu/Conv2d_22.oa.bin"
#define YOLOV3_GOLDEN1_FILE	"/usr/bin/npu/Conv2d_66.oa.bin"
#define YOLOV3_GOLDEN2_FILE	"/usr/bin/npu/Conv2d_58.oa.bin"
#define YOLOV3_GOLDEN3_FILE	"/usr/bin/npu/Conv2d_73.swap.oa.bin"

#define DEFAULT_CLASS	1000

#define PAGE_SHIFT 12

#define NSEC_PER_MSEC	1000000L

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
	   	    " \t1 : SSD , 2 : Classifier ,\n"
		    " \t3 : YOLOV2 4 : YOLOV4, \n" 
		    " \tdefault : SSD \n"	 
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
int classifier_run(uint8_t *oact_base, int log2_oact_scl, int num_class);

int get_file_size(FILE *fp)
{
	int file_size;

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	return file_size;
}

void post_process(void *oact_base)
{
	run_post_process((void *)oact_base, NULL);
}

int Classification_verify(char *ref_path, char *outbuf)
{
	FILE *ref = fopen(ref_path, "ro");
	uint8_t *ref_buf;
	int input_size, ret;

	input_size = get_file_size(ref);
	
	ref_buf = (uint8_t *) malloc(input_size * sizeof(uint8_t));
	ret =  fread(ref_buf, 1, input_size, ref);
	if (ret < 0)
		printf(" Read fail Score Golden data\n");
	ret = memcmp(ref_buf, outbuf, input_size);
	if(!ret)
		printf("Output file Compare OK!\n");
	else
		printf("Output file Compare diff!\n");

	return ret;
}

int SSD_verify(char *outbuf, char *location, char *score)
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
	char *ref_buf;
	int input_size, ret;

	input_size = get_file_size(ref);

	ref_buf = (char *) malloc(input_size * sizeof(char));
	ret =  fread(ref_buf, 1, input_size, ref);
	if (ret < 0)
		printf(" Read fail yolov2 Golden data\n");

	ret = memcmp(ref_buf, outbuf, input_size);
	if(!ret)
		printf("Verify Ok!\n");
	else
		printf("Verify Diff!\n");
	
	free(ref_buf);
	fclose(ref);

	return ret;
}

int YOLOV3_verify(char *outbuf, char *ref1path, char *ref2path,char *ref3path )
{
	FILE *ref1 = fopen(ref1path, "rb");
	FILE *ref2 = fopen(ref2path, "rb");
	FILE *ref3 = fopen(ref3path, "rb");
	char *ref_buf1;
	char *ref_buf2;
	char *ref_buf3;
	int input_size, ret;

	input_size = get_file_size(ref1);

	ref_buf1 = (char *) malloc(input_size * sizeof(char));
	ret =  fread(ref_buf1, 1, input_size, ref1);
	if (ret < 0)
		printf(" Read fail yolov2 Golden data\n");

	ret = memcmp(ref_buf1, outbuf, input_size);
	if(ret) {
		printf("Conv66 Diff!\n");
		goto exit;
	}

	input_size = get_file_size(ref2);
	ref_buf2 = (char *) malloc(input_size * sizeof(char));
	ret =  fread(ref_buf2, 1, input_size, ref2);
	if (ret < 0)
		printf(" Read fail yolov2 Golden data\n");

	ret = memcmp(ref_buf2, outbuf + 0x51380, input_size);
	if(ret){
		printf("Conv58 Diff!\n");
	}

	input_size = get_file_size(ref3);
	ref_buf3 = (char *) malloc(input_size * sizeof(char));
	ret =  fread(ref_buf3, 1, input_size, ref3);
	if (ret < 0)
		printf(" Read fail yolov2 Golden data\n");

	ret = memcmp(ref_buf3, outbuf + 0x10d80, input_size);
	if(ret){
		printf("Conv73 Diff!\n");
		goto exit;
	}
	
	printf("Verify Ok!\n");

exit:
	if(ref_buf1)
		free(ref_buf1);

	if(ref_buf2)
		free(ref_buf2);

	if(ref_buf3)
		free(ref_buf3);

	fclose(ref1);
	fclose(ref2);
	fclose(ref3);

	return ret;
}

int main(int argc, char **argv)
{
	int fd;
	int opt;
	int verify = 0;
	char *in_buf = NULL, *out_buf = NULL;
	char *cmd_buf = NULL, *wei_buf = NULL;
	char *cn1_buf = NULL, *cn0_buf = NULL;
	char *act_buf = NULL;
	
	int input_size, output_size =0x30000, cmp_size = 0x17700;
	char inputfile[128] = INPUT_FILE;
	char cmdfile[128] = CMD_FILE;
	char weightfile[128] = WEIGHT_FILE;
	char score_file[128] = SCORE_FILE;
	char location_file[128] = LOCATION_FILE;
	char classfier_file[128] = CLASS_GOLDEN_FILE;
	char yolov2_file[128] = YOLOV2_GOLDEN_FILE;	
	char yolov3_file1[128] = YOLOV3_GOLDEN1_FILE;	
	char yolov3_file2[128] = YOLOV3_GOLDEN2_FILE;	
	char yolov3_file3[128] = YOLOV3_GOLDEN3_FILE;	
	int post = 1, class = DEFAULT_CLASS;

	FILE *cmd_fp = NULL;
	FILE *wei_fp = NULL;
	FILE *in_fp = NULL;
	FILE *cn1_fp = NULL;
	FILE *cn0_fp = NULL;

	FILE *save = fopen("/usr/bin/npu/save.bin", "wr");
	int ret;

	while(-1 != (opt = getopt(argc, argv, "i:c:w:l:s:vhp:n:"))) {
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
		} 
	}	
	printf("NPU Test Application\n");


	fd = open("/dev/npu", O_RDWR);
	if (fd < 0) {
		printf(" open fail NPU Driver\n");
		exit(0);
	}

	in_fp = fopen(inputfile, "ro");
	if (!in_fp) {
		printf("input file open Error!\n");
		goto end;
	}

	cmd_fp = fopen(cmdfile, "ro");
	if (!cmd_fp) {
		printf("input file open Error!\n");
		goto end;
	}

	wei_fp = fopen(weightfile, "ro");
	if (!wei_fp) {
		printf("input file open Error!\n");
		goto end;
	}
   
	wei_buf = (char*)mmap((void*)0, quant_network_byte_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 1 << PAGE_SHIFT);
	input_size = get_file_size(wei_fp);

	ret = fread(wei_buf, 1, input_size, wei_fp);
	if (ret < 0) {
		printf("Read Wegiht/bias file fail\n");
		goto end;
	}
	msync(wei_buf,quant_network_byte_size, MS_SYNC);

	input_size = get_file_size(cmd_fp);
	cmd_buf = (char*)mmap((void*)0, npu_cmd_code_byte_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0 << PAGE_SHIFT);
	ret =fread(cmd_buf, 1, input_size, cmd_fp);
	if (ret < 0) {
		printf("Read Command file fail\n");
		goto end;
	}
	msync(cmd_buf, npu_cmd_code_byte_size, MS_SYNC);

	act_buf = (char*)mmap((void*)0,  work_buf_byte_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 4 << PAGE_SHIFT);
	msync(act_buf, work_buf_byte_size, MS_SYNC);
	
		
	input_size = get_file_size(in_fp);
	in_buf = (char*)mmap((void*)0, input_buf_byte_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 2 << PAGE_SHIFT);
	ret =  fread(in_buf, 1, input_size, in_fp);
	if (ret < 0) {
		printf("Read input file fail\n");
		goto end;
	}
	msync(in_buf, input_buf_byte_size, MS_SYNC);

	out_buf = (char*)mmap((void*)0, output_buf_byte_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 3 << PAGE_SHIFT);
	msync(out_buf, output_buf_byte_size, MS_SYNC);

	
	printf("write input image\n");
 
	sync();

	system("echo 1 > /proc/sys/vm/drop_caches");

	if (ioctl(fd, NPU_IOCTL_RUN))
	{
		printf("NPU TEST RUN FAIL\n");
		goto end;  
	}	

	msync(out_buf, output_buf_byte_size, MS_SYNC);

	if (post == POST_CLASS) 
		printf("Class : %d\n",classifier_run((uint8_t *)out_buf, 1000, class));
	else
		run_post_process((void *)out_buf, NULL);
	
	
	if (verify) { 
		switch (post) {
		case POST_SSD:
			SSD_verify(out_buf, location_file, score_file);
		break;
		case POST_CLASS :
			Classification_verify(classfier_file, out_buf);
		break;
		case POST_YOLOV2:
			YOLOV2_verify(out_buf, yolov2_file);
		break;
		case POST_YOLOV3:
			YOLOV3_verify(out_buf, yolov3_file1,
				yolov3_file2, yolov3_file3);
		break;
		}

	}

	printf("TEST Done!\n");

end:
	if (in_fp)
		fclose(in_fp);
	if (cmd_fp)
		fclose(cmd_fp);
	if (wei_fp)
		fclose(wei_fp);

	
	if (in_buf)
		munmap(in_buf, input_buf_byte_size);

	if (out_buf)
		munmap(out_buf, output_buf_byte_size);

	if (cmd_buf)
		munmap(cmd_buf, npu_cmd_code_byte_size);

	if (wei_buf)
		munmap(wei_buf, quant_network_byte_size);
	
	if (act_buf)
		munmap(act_buf, work_buf_byte_size);

	close(fd);

	return 0;
}
