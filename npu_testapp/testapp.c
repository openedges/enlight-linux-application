#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "npu.h"

#define INPUT_FILE      "/usr/bin/npu/Relu_0.ia.bin"
#define CMD_FILE        "/usr/bin/npu/npu_cmd.bin"
#define WEIGHT_FILE     "/usr/bin/npu/quantized_network.h.bin"
#define CONCAT_1_FILE   "/usr/bin/npu/Concat_1.oa.bin"
#define CONCAT_0_FILE   "/usr/bin/npu/Concat_0.oa.bin"

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
        , CONCAT_0_FILE
        , CONCAT_1_FILE
    );
}
void run_post_process(void *oact_base);

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
    char *in_buf, *out_buf;
    char *cmd_buf, *wei_buf;
    char *cn1_buf, *cn0_buf;
    int input_size, output_size =0x30000, cmp_size = 0x18000;
    int ret;
    FILE *cn1_fp;
    FILE *cn0_fp;
    char inputfile[128] = INPUT_FILE;
    char cmdfile[128] = CMD_FILE;
    char weightfile[128] = WEIGHT_FILE;
    char concat_1_file[128] = CONCAT_1_FILE;
    char concat_0_file[128] = CONCAT_0_FILE;

    struct npu_buffer_info cmd_info;

    while(-1 != (opt = getopt(argc, argv, "i:c:w:l:s:vh"))) {
        switch (opt) {
        case 'i':   strcpy(inputfile, optarg);      break;
        case 'c':   strcpy(cmdfile, optarg);        break;
        case 'w':   strcpy(weightfile, optarg);     break;
        case 'l':   strcpy(concat_0_file, optarg);  break;
        case 's':   strcpy(concat_1_file, optarg);  break;
        case 'v':   verify = 1;                     break;
        case 'h':   print_usage(); exit(0);         break;
        } 
    }    
    printf("Devcie Driver Test Application\n");

    fd = open("/dev/npu", O_RDWR);
    if (fd < 0) {
        printf(" open fail NPU Driver\n");
        exit(0);
    }

    FILE *in_fp = fopen(inputfile, "ro");
    if (!in_fp) {
        printf("input file open Error!\n");
        goto end;
    }
    FILE *cmd_fp = fopen(cmdfile, "ro");
    if (!cmd_fp) {
        printf("input file open Error!\n");
        goto end;
    }
    FILE *wei_fp = fopen(weightfile, "ro");
    if (!wei_fp) {
        printf("input file open Error!\n");
        goto end;
    }

    
    input_size = get_file_size(cmd_fp);
    cmd_buf = (char *) malloc(input_size* sizeof( char));
    ret =fread(cmd_buf, 1, input_size, cmd_fp);
    if (ret < 0) {
        printf("Read Command file fail\n");
        goto end;
    }
    cmd_info.buffer = cmd_buf;
    cmd_info.size = input_size;

    ret = ioctl(fd, NPU_IOCTL_SET_CMD_BUF, &cmd_info);
    if (ret) {
        printf("Set Command buf set fail\n");
        goto end;
    }


    input_size = get_file_size(wei_fp);

    wei_buf = (char *) malloc(input_size * sizeof(char));
    ret = fread(wei_buf, 1, input_size, wei_fp);
    if (ret < 0) {
        printf("Read Wegiht/bias file fail\n");
        goto end;
    }
    cmd_info.buffer = wei_buf;
    cmd_info.size = input_size;
    ret = ioctl(fd, NPU_IOCTL_SET_WEIGHT_BUF, &cmd_info);
    if (ret) {
        printf("Wegiht/bias buf set fail\n");
        goto end;
    }

    input_size = get_file_size(in_fp);

    in_buf = (char *) malloc(input_size * sizeof(char));
    out_buf = (char *) malloc(output_size * sizeof(char));

    ret =  fread(in_buf, 1, input_size, in_fp);
    if (ret < 0) {
        printf("Read input file fail\n");
        goto end;
    }
               

    printf("write input image\n");
    write(fd, in_buf, input_size);
 
    sync();
    system("echo 1 > /proc/sys/vm/drop_caches");
    if (ioctl(fd, NPU_IOCTL_RUN))
    {
        printf("NPU TEST FAIL\n");
        goto end;     
    }    
    
    while(1)
    {
        int i;
        ret = read(fd, out_buf, output_size);
        if (ret > 0) {
            printf("read success %d\n", ret); 
            break;
        }
    }
  
    if (verify) { 
        cn1_fp = fopen(concat_1_file, "ro");
        cn0_fp = fopen(concat_0_file, "ro");


        input_size = get_file_size(cn1_fp);
   
        cn1_buf = (char *) malloc(input_size * sizeof(char));
        ret =  fread(cn1_buf, 1, input_size, cn1_fp);
        if (ret < 0)
            printf(" Read fail Concat_1 Golden data\n");

        input_size = get_file_size(cn0_fp);

        cn0_buf = (char *) malloc(input_size  * sizeof(char));
        ret = fread(cn0_buf, 1, input_size, cn0_fp);

        if (ret < 0)
            printf(" Read fail Concat_0 Golden data\n");

        
        ret = memcmp(cn1_buf, out_buf, input_size);
        if(!ret)
           printf("Concat 1 Compare Success!\n");
        else
           printf("Concat 1 Compare Fail!\n");
 
        ret = memcmp(cn0_buf, out_buf + 0x18000, input_size);
        if(!ret)
           printf("Concat 0 Compare Success!\n");
        else
           printf("Concat 0 Compare Fail!\n");
   }
   run_post_process(out_buf);
   printf("TEST Done!\n");

end:
    if (in_fp)
        fclose(in_fp);
    if (cmd_fp)
        fclose(cmd_fp);
    if (wei_fp)
        fclose(wei_fp);

    
    if (in_buf)
        free(in_buf);

    if (out_buf)
        free(out_buf);

    if (cmd_buf)
        free(cmd_buf);

    if (wei_buf)
        free(wei_buf);
   
     if (verify) {
        fclose(cn1_fp);
        fclose(cn0_fp);
        free(cn0_buf);
        free(cn1_buf);
    }

    close(fd);

    return 0;
}
