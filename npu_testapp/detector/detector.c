#include "enlight_network.h"
#include "detector.h"

#define MALLOC_UNIT (2*256*1024)
#define MALLOC_NUM (16)

#ifdef SUPPORT_MALLOC
void* enlight_malloc(int size)
{
    return (uint8_t*)malloc(size);
}

void enlight_free(void* buf)
{
    free(buf);
}
#else

#ifdef TEST_ON_EMBEDDED_LINUX
#ifdef DUMP
void enlight_dump_data_to_bin(char *fn, void *buf, int size_of_dtype, int num_ele)
{
    FILE *fp = fopen(fn, "wb");
    fwrite(buf, size_of_dtype, num_ele, fp);
    fclose(fp);
}
#endif
#endif

uint8_t* malloc_buf;
int malloc_buf_used[MALLOC_NUM];

static void enlight_mem_status()
{
#if 0
    int i;
    enlight_detector_err("enlight_free: buf_used: ");
    for (i = 0 ; i < MALLOC_NUM ; i++) {
        enlight_detector_err("%d ", malloc_buf_used[i]);
    }
    enlight_detector_err("\n");
#endif
}

void enlight_init_mem(void *buf)
{
    int i;
    malloc_buf = (uint8_t*)buf;
    for (i = 0 ; i < MALLOC_NUM ; i++)
        malloc_buf_used[i] = 0;
}

void* enlight_malloc(int size)
{
    int* ptr;
    int i;

    for (i = 0 ; i < MALLOC_NUM ; i++) {
        if (!malloc_buf_used[i]) {
            malloc_buf_used[i] = 1;

            ptr = (int*)(malloc_buf + i*(MALLOC_UNIT+8));
            *ptr = i;

            enlight_mem_status();

            return (uint8_t*)(ptr + 2);
        }
    }

    enlight_detector_err("malloc failed\n");
    return NULL;
}

void enlight_free(void* buf)
{
    int* ptr = (int*)buf;
    malloc_buf_used[ptr[-2]] = 0;

    enlight_mem_status();
}

#endif

static int get_data_size(enlight_data_t dtype)
{
    switch (dtype) {
        case ENLIGHT_DTYPE_UINT8:
        case ENLIGHT_DTYPE_INT8:
            return 1;
        case ENLIGHT_DTYPE_UINT16:
        case ENLIGHT_DTYPE_INT16:
            return 2;
        case ENLIGHT_DTYPE_UINT32:
        case ENLIGHT_DTYPE_INT32:
            return 4;
        case ENLIGHT_DTYPE_FLOAT:
            return 8;
        default:
            return 0;
    }
}

static void swap(int* l, int* r)
{
    int tmp;

    tmp = *l;
    *l = *r;
    *r = tmp;
}

tensor_t* alloc_tensor(int dtype, int num_dim, int* dims, float scl)
{
    int i;
    int num_ele = 1;

    for (i = 0; i < num_dim; i++)
        num_ele *= dims[i];

    int unit_size = get_data_size(dtype);
    uint8_t* buf = (uint8_t*)enlight_malloc(sizeof(tensor_t) + unit_size * num_ele);

    tensor_t* tensor = (tensor_t*)buf;
    tensor->buf = (uint8_t*)(buf + sizeof(tensor_t));

    tensor->scl = scl;
    tensor->dtype = dtype;
    tensor->num_ele = num_ele;
    tensor->num_dim = num_dim;

    for (i = 0 ; i < num_dim; i++)
        tensor->dims[i] = dims[i];

    tensor->sig_tbl = NULL;
    tensor->exp_tbl = NULL;
    return tensor;
}

void free_tensor(tensor_t* tensor)
{
    enlight_free(tensor);
}

detection_t* alloc_detection(int max_detection)
{
    detection_t* detection;
    uint8_t* buf;

    int size = sizeof(detection_t);

    size += sizeof(box_t) * max_detection;
    size += sizeof(uint16_t) * max_detection;
    size += sizeof(uint16_t) * max_detection;

    buf = (uint8_t*)enlight_malloc(size);

    detection = (detection_t*)buf;

    buf += sizeof(detection_t);
    detection->box = (box_t*)buf;

    buf += sizeof(box_t) * max_detection;
    detection->score = (uint16_t*)buf;

    buf += sizeof(uint16_t) * max_detection;
    detection->cls = (uint16_t*)buf;

    detection->n = 0;

    return detection;
}

void free_detection(detection_t* detection)
{
    enlight_free(detection);
}

static void copy_box(box_t* dst, box_t* src)
{
    dst->x_min = src->x_min;
    dst->x_max = src->x_max;
    dst->y_min = src->y_min;
    dst->y_max = src->y_max;
}

void copy_tensor_info(tensor_t* dst, tensor_t* src)
{
    dst->scl = src->scl;
    dst->dtype = src->dtype;

    dst->num_ele = src->num_ele;
    dst->num_dim = src->num_dim;

    dst->dims[0] = src->dims[0];
    dst->dims[1] = src->dims[1];
    dst->dims[2] = src->dims[2];
    dst->dims[3] = src->dims[3];
}

#define MAX_CLASS_NUM 1024
void softmax(tensor_t* src, tensor_t* dst, int stride)
{
    //enlight_detector_dbg_fin();

    int i, c;
    unsigned long e, sum;
    unsigned long e_array[MAX_CLASS_NUM];

    int num_box = src->dims[1];
    int num_cls = src->dims[2];

    int8_t* cls_conf = src->buf;
    uint32_t* exp_tbl = (uint32_t*)src->exp_tbl;
    uint32_t* conf = (uint32_t*)dst->buf;

    int dst_scl = (int)(dst -> scl);

    for (i = 0 ; i < num_box; i++) {

        sum = 0;

        for (c = 0 ; c < num_cls ; c++) {
            e = exp_tbl[cls_conf[c]];
            e_array[c] = e;
            sum += e;
        }

        if (sum > 0) {
            for (c = 0 ; c < num_cls ; c++)
                *conf++ = (uint32_t)MIN((e_array[c] * dst_scl) / sum, dst_scl - 1);
        }
        cls_conf += stride;
    }

    //enlight_detector_dbg_fout();

}

int softmax_with_threshold(
    tensor_t* src,
    tensor_t* dst,
    int *cand_box_index_buf, 
    int stride,
    float threshold,
    int max_cand_box)
{
    enlight_detector_dbg_fin();

    int i;

    // batches, num_boxes, num_clses
    int num_box = src->dims[1];
    int num_cls = src->dims[2];

    enlight_act_tensor_t* output_tensor = (enlight_act_tensor_t*)src->output_tensor;

    int8_t* cls_conf;
    int8_t* cls_conf_base;

    uint32_t* exp_tbl = (uint32_t*)src->exp_tbl;
    uint32_t* cand_conf = (uint32_t*)dst->buf;

    int num_cand_box = 0;
    int dims[4];

    if (num_cls > MAX_CLASS_NUM) {
        enlight_detector_err("error: num_cls %d > %d\n", num_cls, MAX_CLASS_NUM);
    }

    enlight_get_tensor_dimensions(output_tensor, dims);

    cls_conf = (int8_t*)enlight_malloc(dims[1] * dims[2] * dims[3]);
    cls_conf_base = cls_conf;
    enlight_get_tensor_qdata(output_tensor, cls_conf);

    int cand_conf_scl = (int)(dst -> scl);

    for (i = 0; i < num_box; i++) {
        unsigned long e, sum;
        unsigned long e_array[MAX_CLASS_NUM], sum_x_th;
        int c, valid_box;

        sum = 0;
        valid_box = 0;

        for (c = 0; c < num_cls; c++) {
            e = exp_tbl[cls_conf[c]];
            e_array[c] = e;
            sum += e;
        }

        if (sum > 0) {
            sum_x_th = ((unsigned long)(sum * threshold)) ;

            for (c = 1; c < num_cls; c++) {

                if (e_array[c] >= sum_x_th) {
                    // clear buffer once
                    if (valid_box == 0) {
                        int j;
                        for (j = 0; j < num_cls; j++)
                            cand_conf[num_cand_box * num_cls + j] = 0;
                        valid_box = 1;
                    }

                    cand_conf[num_cand_box * num_cls + c] = (uint32_t)MIN((e_array[c] * cand_conf_scl) / sum, cand_conf_scl - 1);

                    //enlight_detector_dbg("cand_conf[%d]: %d\n",
                    //    num_cand_box * num_cls + c,
                    //    cand_conf[num_cand_box * num_cls + c]);
                }
            }

            if (valid_box) {
                num_cand_box++;
                *cand_box_index_buf++ = i;
            }
        }

        cls_conf += stride;

        if (num_cand_box >= max_cand_box) {
            //enlight_detector_dbg("candidate box num over %d > %d\n", num_cand_box, max_cand_box);
            break;
        }
    }

    enlight_free(cls_conf_base);

    enlight_detector_dbg_fout();

    return num_cand_box;
}

/** @brief return MIN((Intersect / Union * 1024), 1023)
*/
float iou(box_t* l, box_t* r)
{
    //enlight_detector_dbg_fin();

    int area_union;
    int area_inter;

    int x1, x2;
    int y1, y2;
    int w, h;
    int w1, w2, h1, h2;

    x1 = MAX(l->x_min, r->x_min);
    x2 = MIN(l->x_max, r->x_max);
    y1 = MAX(l->y_min, r->y_min);
    y2 = MIN(l->y_max, r->y_max);

    w = MAX(x2 - x1, 0);
    h = MAX(y2 - y1, 0);

    area_inter = w * h;

    w1 = l->x_max - l->x_min;
    h1 = l->y_max - l->y_min;

    w2 = r->x_max - r->x_min;
    h2 = r->y_max - r->y_min;

    area_union = w1*h1 + w2*h2 - area_inter;

    // to avoid divide by zero
    if (area_union == 0)
        return 1023./1024;
    else
        return MIN((((float)area_inter) / area_union), 1023./1024);

    //enlight_detector_dbg_fout();
}


void nms(detection_t* detection, cand_t* cand, int cls, float th)
{
    //enlight_detector_dbg_fin();

    int i, j, score, n = detection->n;
    int num_cand = cand->n;
    int ref_index, test_index;

    int* indice    = (int*)enlight_malloc(sizeof(int)*num_cand);
    char* deleted  = (char*)enlight_malloc(sizeof(char)*num_cand);

    box_t* cand_box = cand->box;
    uint16_t* cand_score = cand->score;

    box_t* detection_box = detection->box;
    uint16_t* detection_cls = detection->cls;
    uint16_t* detection_score = detection->score;

    for (i = 0 ; i < num_cand ; i++){
        indice[i]   = i;
        deleted[i]  = 0;
    }

    intro_sort(cand_score, indice, num_cand);

    //indices are sorted ascending order
    for (i = num_cand - 1; i >=0; i--) {
        ref_index = indice[i];
        if (!deleted[i]) {
            score = cand_score[ref_index];

            // add detection result
            copy_box(&detection_box[n], &cand_box[ref_index]);
            detection_cls[n] = cls;
            detection_score[n] = score;

            deleted[i] = 1;
            n++;
        }

        // non-maximum surpression
        for (j = i - 1; j >= 0; j--) {
            test_index = indice[j];
            if (!deleted[j]) {
                if (iou(&cand_box[ref_index], &cand_box[test_index]) >= th)
                    deleted[j] = 1;
            }
        }
    }

    detection->n = n;

    enlight_free(indice);
    enlight_free(deleted);

    //enlight_detector_dbg_fout();
}

void fastnms(detection_t* detection, cand_t* cand, float th)
{
    int i, j, score, class, n = detection->n;
    int num_cand = cand->n;
    int ref_index, test_index;

    int* indice    = (int*)enlight_malloc(sizeof(int)*num_cand);
    char* deleted  = (char*)enlight_malloc(sizeof(char)*num_cand);

    box_t* cand_box = cand->box;
    uint16_t* cand_score = cand->score;
    uint16_t* cand_class = cand->cls;

    box_t* detection_box = detection->box;
    uint16_t* detection_class = detection->cls;
    uint16_t* detection_score = detection->score;

    for (i = 0 ; i < num_cand ; i++){
        indice[i]   = i;
        deleted[i]  = 0;
    }

    intro_sort(cand_score, indice, num_cand);

    //indices are sorted ascending order
    for (i = num_cand - 1; i >=0; i--) {
        ref_index = indice[i];
        if (!deleted[i]) {
            score = cand_score[ref_index];
            class = cand_class[ref_index];

            // add detection result
            copy_box(&detection_box[n], &cand_box[ref_index]);
            detection_class[n] = class;
            detection_score[n] = score;

            deleted[i] = 1;
            n++;

            // non-maximum surpression
            for (j = i - 1; j >= 0; j--) {
                test_index = indice[j];
                if (!deleted[j]) {
                    if (iou(&cand_box[ref_index], &cand_box[test_index]) >= th)
                        deleted[j] = 1;
                }
            }
        }
    }

    detection->n = n;

    enlight_free((uint8_t*)indice);
    enlight_free((uint8_t*)deleted);
}

void sigmoid(tensor_t *src, tensor_t *dst, int stride)
{
    //enlight_detector_dbg_fin();

    int i, c;

    // batches, num_boxes, num_clses
    int num_box = src->dims[1];
    int num_cls = src->dims[2];

    int8_t* cls_conf = src->buf;
    uint32_t* sig_tbl = (uint32_t*)src->sig_tbl;
    uint32_t* conf = (uint32_t*)dst->buf;

    if (num_cls > MAX_CLASS_NUM) {
        enlight_detector_err("error: num_cls > %d\n", MAX_CLASS_NUM);
    }

    for (i = 0 ; i < num_box; i++) {
        //enlight_detector_dbg("\n");
        for (c = 0 ; c < num_cls ; c++) {
            uint32_t val = sig_tbl[cls_conf[c]];
            *conf++ = val;
            //enlight_detector_dbg("%4d %4d,", cls_conf[c], val);
        }
        cls_conf += stride;
    }

    //enlight_detector_dbg_fout();
}


int sigmoid_with_threshold(tensor_t *src, tensor_t *dst, int *cand_box,
                           int stride, float threshold,
                           int has_bg, int max_nms_cand, int sigmoid_applied)
{
    int i, c, n = 0;

    // batches, num_boxes, num_classes
    int num_box = src->dims[1];
    int num_class = src->dims[2];

    int8_t *cls_conf, *cls_conf_base;

    enlight_act_tensor_t* output_tensor = (enlight_act_tensor_t*)src->output_tensor;

    uint32_t* sig_tbl = (uint32_t*)src->sig_tbl;
    uint32_t* conf = (uint32_t*)dst->buf;

    if (num_class > MAX_CLASS_NUM) {
        enlight_detector_err("error: num_class > %d\n", MAX_CLASS_NUM);
    }

    int dims[4];
    enlight_get_tensor_dimensions(output_tensor, dims);

    cls_conf = (int8_t*)enlight_malloc(dims[1] * dims[2] * dims[3]);
    cls_conf_base = cls_conf;
    enlight_get_tensor_qdata(output_tensor, cls_conf);

    uint32_t sig_value;
    uint32_t sig_value_buffer[MAX_CLASS_NUM];
    int valid_box;

    int th_ = (int)(threshold * dst->scl);

    for (i = 0; i < num_box; i++) {
        valid_box = 0;

        for (c = has_bg; c < num_class ; c++) {
            sig_value = cls_conf[c];
            if (!sigmoid_applied)
                sig_value = sig_tbl[cls_conf[c]];

            if (sig_value >= th_) {
                valid_box = 1;
            }
            else {
                // accordance with dumping tensor from python
                sig_value = 0;
            }

            sig_value_buffer[c] = sig_value;
        }

        if (valid_box) {

            for (c = has_bg; c < num_class ; c++)
                conf[n * num_class + c] = sig_value_buffer[c];

            cand_box[n++] = i;
        }

        cls_conf += stride;

        if (n == max_nms_cand)
            break;
    }

    enlight_free(cls_conf_base);

    return n;
}

void heap_sort(const uint16_t* unsorted, int* indices, int N)
{
    N = N - 1;

    int tree_index, parent, child;
    int temp_data, temp_index;

    for (tree_index = (N - 1) >> 1; tree_index >= 0; tree_index--) {
        temp_index = indices[tree_index];
        temp_data = unsorted[temp_index];
        
        for (parent = tree_index, child = (tree_index << 1) + 1; child <= N;) {
            if (child < N) {
                if (unsorted[indices[child]] < unsorted[indices[child + 1]])
                    child++;
            }
            if (temp_data < unsorted[indices[child]]) {
                indices[parent] = indices[child];
                parent = child;
                child += (child + 1);
            }
            else
                break;
        }
        indices[parent] = temp_index;
    }

    do {
        temp_index = indices[N];
        temp_data = unsorted[temp_index];
        indices[N] = indices[0];
        N--;
        for (parent = 0, child = 1; child <= N;) {
            if (child < N) {
                if (unsorted[indices[child]] < unsorted[indices[child + 1]])
                    child++;
            }
            if (temp_data < unsorted[indices[child]]) {
                indices[parent] = indices[child];
                parent = child;
                child += (child + 1);
            }
            else
                break;
        }
        indices[parent] = temp_index;
    } while (N > 0);
}


void insertion_sort(const uint16_t* unsorted, int* indices, int N)
{
    int i, j;
    int ref_data, ref_idx, test_data, test_idx;

    for (i = 1; i < N; i++) {
        ref_idx = indices[i];
        ref_data = unsorted[ref_idx];

        for (j = i - 1; j >= 0; j--) {
            test_idx = indices[j];
            test_data = unsorted[test_idx];
            if (ref_data < test_data)
                indices[j + 1] = indices[j];
            else
                break;
        }
        indices[j + 1] = ref_idx;
    }

}


int get_depth(int num_ele)
{
    int depth_limit = 0;
    while (num_ele > 0) {
        num_ele >>= 1;
        depth_limit++;
    }
    return depth_limit;
}


void intro_sort(const uint16_t* unsorted, int* indices, int N)
{
    if(N == 0)
        return;

    int pivot;

    int *i, *j, *left, *middle, *right;
        
    int *indices_stack[MAX_STACK_SIZE];
    int    depth_stack[MAX_STACK_SIZE];
    
    int indices_top = -1;
    int depth_top = -1;
    int depth_limit = get_depth(N) * 2;

    indices_stack[++indices_top] = indices;
    indices_stack[++indices_top] = indices + N - 1;
    depth_stack[++depth_top] = depth_limit;

    while (indices_top > 0) {
        right       = indices_stack[indices_top--];
        left        = indices_stack[indices_top--];
        depth_limit = depth_stack[depth_top--];

        //Heap sort if recursion would be deeper
        if (depth_limit < 0)
            heap_sort(unsorted, left, right - left + 1);
        else {
        //Quick sort if size is more than insertion sort available size
            while (right - left > INSERTION_SORT_SIZE) {
                // three median quick sort
                middle = left + ((right - left) >> 1);
                if (unsorted[*middle] < unsorted[*left])
                    swap(middle, left);
                if (unsorted[*right] < unsorted[*middle])
                    swap(right, middle);
                if (unsorted[*middle] < unsorted[*left])
                    swap(middle, left);

                pivot = unsorted[*middle];
                i = left;
                j = right - 1;
                swap(middle, j);
                do {
                    do {
                        i++;
                    } while (i < j && pivot > unsorted[*i]);
                    do {
                        j--;
                    } while (j > i && pivot < unsorted[*j]);
                    if (i >= j)
                        break;
                    swap(i, j);
                } while (1);
                swap(i, right - 1);

                if (i - left < right - i) {
                    indices_stack[++indices_top] = i + 1;
                    indices_stack[++indices_top] = right;
                    right = i - 1;
                }
                else {
                    indices_stack[++indices_top] = left;
                    indices_stack[++indices_top] = i - 1;
                    left = i + 1;
                }
                depth_stack[++depth_top] = --depth_limit;               
            }
        //else do insertion sort
            insertion_sort(unsorted, left, right - left + 1);
        }
    }
}

