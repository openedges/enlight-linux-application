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

#define INPUT_FILE	"/usr/bin/npu/input.ia.bin"
#define CMD_FILE	"/usr/bin/npu/npu_cmd.bin"
#define WEIGHT_FILE	"/usr/bin/npu/quantized_network.h.bin"
#define SCORE_FILE	"/usr/bin/npu/Concat_269.oa.bin"
#define LOCATION_FILE	"/usr/bin/npu/Concat_270.oa.bin"

#define PAGE_SHIFT 12

#define NSEC_PER_MSEC	1000000L

struct npu_buffer_info {
	void *buffer;
	unsigned int size;
};

void print_usage(void)
{
	printf(" usage : options\n"
		    " -i : input image               , default : %s  \n"
		    " -c : Network Command file      , default : %s \n"
		    " -w : Network Weight/bias file  , default : %s \n"
		    " -l : Concat_0 golden data file , default : %s  \n"
		    " -s : Concat_1 golden data file , default : %s  \n"
		    " -v : verification out tensor                   \n"
		, INPUT_FILE
		, CMD_FILE
		, WEIGHT_FILE
		, LOCATION_FILE
		, SCORE_FILE
	);
}
void run_post_process(void *oact_base, void *work_base);

int get_file_size(FILE *fp)
{
	int file_size;

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	return file_size;
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
	char *test_buf; // for post process work buffer
	
	int input_size, output_size =0x30000, cmp_size = 0x17700;
	int ret;
	FILE *cn1_fp = NULL;
	FILE *cn0_fp = NULL;
	char inputfile[128] = INPUT_FILE;
	char cmdfile[128] = CMD_FILE;
	char weightfile[128] = WEIGHT_FILE;
	char score_file[128] = SCORE_FILE;
	char location_file[128] = LOCATION_FILE;

	FILE *cmd_fp = NULL;
	FILE *wei_fp = NULL;
	FILE *in_fp = NULL;
	struct npu_buffer_info cmd_info;

	while(-1 != (opt = getopt(argc, argv, "i:c:w:l:s:vh"))) {
		switch (opt) {
		case 'i':   strcpy(inputfile, optarg);      break;
		case 'c':   strcpy(cmdfile, optarg);        break;
		case 'w':   strcpy(weightfile, optarg);     break;
		case 'l':   strcpy(location_file, optarg);  break;
		case 's':   strcpy(score_file, optarg);  break;
		case 'v':   verify = 1;                     break;
		case 'h':   print_usage(); exit(0);         break;
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
   
	wei_buf = (char*)mmap((void*)0, 5 * 1024 * 1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 1 << PAGE_SHIFT);
	input_size = get_file_size(wei_fp);

	ret = fread(wei_buf, 1, input_size, wei_fp);
	if (ret < 0) {
		printf("Read Wegiht/bias file fail\n");
		goto end;
	}
	msync(wei_buf, 5 * 1024 * 1024, MS_SYNC);

	input_size = get_file_size(cmd_fp);
	cmd_buf = (char*)mmap((void*)0,  512 * 1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0 << PAGE_SHIFT);
	ret =fread(cmd_buf, 1, input_size, cmd_fp);
	if (ret < 0) {
		printf("Read Command file fail\n");
		goto end;
	}
	msync(cmd_buf, 512 * 1024, MS_SYNC);

	act_buf = (char*)mmap((void*)0,  15 * 1024 * 1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 4 << PAGE_SHIFT);
	msync(act_buf, 15 * 1024 * 1024, MS_SYNC);
	
		
	input_size = get_file_size(in_fp);
	in_buf = (char*)mmap((void*)0, 512 * 1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 2 << PAGE_SHIFT);
	ret =  fread(in_buf, 1, input_size, in_fp);
	if (ret < 0) {
		printf("Read input file fail\n");
		goto end;
	}
	msync(in_buf, 512 * 1024, MS_SYNC);

	out_buf = (char*)mmap((void*)0, 512 * 1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 3 << PAGE_SHIFT);
	msync(out_buf, 512, 1024);


	test_buf = (char *) malloc(80 * 1024  * 1024 * sizeof(char));

	
	printf("write input image\n");
 
	sync();

	system("echo 1 > /proc/sys/vm/drop_caches");

	if (ioctl(fd, NPU_IOCTL_RUN))
	{
		printf("NPU TEST RUN FAIL\n");
		goto end;  
	}	

	msync(out_buf, 512 * 1024, MS_SYNC);


	if (verify) { 
		cn1_fp = fopen(score_file, "ro");
		cn0_fp = fopen(location_file, "ro");

		input_size = get_file_size(cn1_fp);

		cn1_buf = (char *) malloc(input_size * sizeof(char));
		ret =  fread(cn1_buf, 1, input_size, cn1_fp);
		if (ret < 0)
			printf(" Read fail Concat_1 Golden data\n");

		memcpy(test_buf, cn1_buf, input_size);
		ret = memcmp(cn1_buf, test_buf, input_size);

		input_size = get_file_size(cn0_fp);

		cn0_buf = (char *) malloc(input_size  * sizeof(char));
		ret = fread(cn0_buf, 1, input_size, cn0_fp);

		if (ret < 0)
			printf(" Read fail Concat_0 Golden data\n");

		memcpy(test_buf+0x18700, cn0_buf, input_size);
		
		ret = memcmp(cn1_buf, out_buf, input_size);
		if(!ret)
			printf("Score Compare Success!\n");
		else
			printf("Score Compare Fail!\n");
 
		ret = memcmp(cn0_buf, out_buf + 0x18700, input_size);
		if(!ret)
			printf("LOCATION Compare Success!\n");
		else
			printf("LOCATION Compare Fail!\n");
	}

	run_post_process(out_buf,test_buf );

	printf("TEST Done!\n");

end:
	if (in_fp)
		fclose(in_fp);
	if (cmd_fp)
		fclose(cmd_fp);
	if (wei_fp)
		fclose(wei_fp);

	
	if (in_buf)
		munmap(in_buf, 512 * 1024);

	if (out_buf)
		munmap(out_buf, 512 * 1024);

	if (cmd_buf)
		munmap(cmd_buf, 512 * 1024);

	if (wei_buf)
		munmap(wei_buf, 6* 1024 * 1024);
	
	if (act_buf)
		munmap(act_buf, 15* 1024 * 1024);

  	
	if (test_buf)
		free(test_buf);
 
	if (verify) {
		fclose(cn1_fp);
		fclose(cn0_fp);
		free(cn0_buf);
		free(cn1_buf);
	}

	close(fd);

	return 0;
}
