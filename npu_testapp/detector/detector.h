#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef TEST_ON_EMBEDDED_LINUX
#   define SUPPORT_MALLOC
#endif

#ifndef MIN
#   define MIN(A, B) (((A)>(B))?(B):(A))
#endif
#ifndef MAX
#   define MAX(A, B) (((A)>(B))?(A):(B))
#endif

#ifdef TEST_ON_EMBEDDED_LINUX
#   define enlight_log(...)  do { printf(__VA_ARGS__); } while(0)
#else
extern int _printf(const char *format, ...);
#   define enlight_log(...)  do { _printf(__VA_ARGS__); } while(0)
#endif

#ifndef NULL
#   define  NULL    0
#endif

#define MAX_CANDIDATE_BOX   16384  // yolov3 max default box num : 13x13x3 = 507, 26x26x3 = 2028, 52x52x3 = 8112 => 10647
#define MAX_DETECT_BOX      256   // max detected box num
#define MAX_STACK_SIZE      64    //Max recursion to Max_CANDIDATE_BOX 2*log2(16384) * element(2)
#define INSERTION_SORT_SIZE 16


#define alloc_candidate(max_candidate) alloc_detection(max_candidate)
#define free_candidate(candidate) free_detection(candidate)

enum {
    TYPE_UINT8,
    TYPE_INT8,
    TYPE_UINT16,
    TYPE_INT16,
    TYPE_INT32,
    TYPE_UINT32,
    TYPE_FLOAT
};


typedef struct {
    int16_t x_min;
    int16_t x_max;
    int16_t y_min;
    int16_t y_max;
    int16_t class;
    int16_t score;
    int16_t img_w;
    int16_t img_h;
} obj_t;

typedef struct {
    int cnt;
    obj_t obj[MAX_DETECT_BOX];
} objs_t;

typedef struct {
    void* buf;

    int scl;

    int dtype;
    int num_ele;
    int num_dim;

    int dims[4];

    void* sig_tbl;
    void* exp_tbl;
} tensor_t;

typedef struct {
    int16_t x_min;
    int16_t x_max;
    int16_t y_min;
    int16_t y_max;
} box_t;

typedef struct {
    box_t* box;

    uint8_t* class;
    uint8_t* score;

    int n;
} detection_t;

typedef struct
{
    float x_min;
    float x_max;
    float y_min;
    float y_max;
} box_f_t;

typedef struct
{
    box_f_t* box;

    uint8_t* class;
    float* score;

    int n;
} detection_f_t;

typedef detection_t cand_t;
typedef detection_f_t cand_f_t;

#ifndef SUPPORT_MALLOC
void enlight_init_mem(void *buf);
#endif
uint8_t* enlight_malloc(int size);
void enlight_free(uint8_t* buf);
tensor_t* alloc_tensor(int dtype, int num_dim, int* dims, int scl);
void free_tensor(tensor_t* tensor);
detection_t* alloc_detection(int detect_num);
void free_detection(detection_t* detection);
void copy_tensor_info(tensor_t* dst, tensor_t* src);
void softmax(tensor_t* src, tensor_t* dst, int stride);
void softmax_with_threshold(tensor_t* src, tensor_t* dst, int *cand_box,
                            int *num_cand_box, int stride, uint32_t threshold,
                            int has_bg, int max_nms_cand);
int iou(box_t* l, box_t* r);
void nms(detection_t* detection, cand_t* cand, int class, uint8_t th);
void fastnms(detection_t* detection, cand_t* cand, uint8_t th);
void sigmoid(tensor_t *src, tensor_t *dst, int stride);
void sigmoid_with_threshold(tensor_t* src, tensor_t* dst, int *cand_box,
                            int *num_cand_box, int stride, uint32_t threshold,
                            int has_bg, int max_nms_cand);
void intro_sort(const uint8_t* unsorted, int* indices, int N);
