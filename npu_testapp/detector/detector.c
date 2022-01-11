#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TENSOR_BUF_STRIDE 32

#ifdef TEST_ON_EMBEDDED_LINUX
#define SUPPORT_MALLOC
//#define MAKE_EXPTABLE
#endif

#ifdef TEST_ON_BARE

#endif

typedef struct
{
    int16_t x_min;
    int16_t x_max;
    int16_t y_min;
    int16_t y_max;
    int16_t class;
    int16_t score;
} obj_t;

typedef struct
{
    int cnt;
    obj_t obj[256];
} objs_t;

objs_t objects;

#ifndef MAKE_EXPTABLE
#   include "exp_tables.h"
#endif

#ifndef TEST_ON_EMBEDDED_LINUX
extern int _printf(const char *format, ...);
#   define enlight_log(...)  do { _printf(__VA_ARGS__); } while(0)
#else
#   define enlight_log(...)  do { printf(__VA_ARGS__); } while(0)

#endif
enum {
    TYPE_UINT8,
    TYPE_INT8,
    TYPE_UINT16,
    TYPE_INT16,
    TYPE_FLOAT
};

typedef struct
{
    void* buf;

    int log2_scale;

    int data_type;
    int num_element;
    int num_dimension;

    int dimensions[4];

    // optional table, ex: table[qvalue] = exp(qvalue)
    void* table;
} tensor_t;

typedef struct
{
    int16_t x_min;
    int16_t x_max;
    int16_t y_min;
    int16_t y_max;
} box_t;

typedef struct
{
    box_t* box;

    uint8_t* class;
    uint8_t* score;

    int n;
} detection_t;

typedef detection_t candidate_t;

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

typedef detection_f_t candidate_f_t;

#ifndef NULL
#define NULL 0
#endif

#define MIN(A, B) (((A)>(B))?(B):(A))
#define MAX(A, B) (((A)>(B))?(A):(B))

#define MALLOC_UNIT (2*256*1024)
#define MALLOC_NUM (8)

#ifndef SUPPORT_MALLOC
//uint8_t malloc_buf[MALLOC_NUM*(MALLOC_UNIT+4)];
uint8_t* malloc_buf;
int malloc_buf_used[MALLOC_NUM]; 
#endif

uint8_t* enlight_malloc(int size)
{
#ifdef SUPPORT_MALLOC
    return (uint8_t*)malloc(size);
#else
    int* ptr;
    int i;

    for (i = 0 ; i < MALLOC_NUM ; i++)
    {
        if (!malloc_buf_used[i])
        {
            malloc_buf_used[i] = 1;

            ptr = (int*)(malloc_buf + i*(MALLOC_UNIT+8));
            *ptr = i;

            //enlight_log("enlight_malloc size %d %d\n", size, i);
            return (uint8_t*)(ptr + 2);
        }
    }

    enlight_log("malloc failed\n");

    return NULL;
#endif
}

void enlight_free(uint8_t* buf)
{
#ifdef SUPPORT_MALLOC
    free(buf);
#else
    int* ptr = (int*)buf;
    malloc_buf_used[ptr[-2]] = 0;
#endif
}

int get_data_size(int data_type)
{
    switch (data_type)
    {
        case TYPE_UINT8:
        case TYPE_INT8:
            return 1;
        case TYPE_UINT16:
        case TYPE_INT16:
            return 2;
        case TYPE_FLOAT:
            return 8;
        default:
            return 0;
    }
}

void swap(int* l, int* r)
{
    int tmp;

    tmp = *l;
    *l = *r;
    *r = tmp;
}

tensor_t* alloc_tensor(int data_type, int num_dimension, int* dimensions, int log2_scale)
{
    int i;
    int num_element = 1;

    for (i = 0 ; i < num_dimension ; i++)
        num_element *= dimensions[i];

    int unit_size = get_data_size(data_type);
    uint8_t* buf = enlight_malloc(sizeof(tensor_t) + unit_size * num_element);

    tensor_t* tensor = (tensor_t*)buf;
    tensor->buf = (uint8_t*)(buf + sizeof(tensor_t));

    tensor->log2_scale = log2_scale;
    tensor->data_type = data_type;
    tensor->num_element = num_element;
    tensor->num_dimension = num_dimension;

    for (i = 0 ; i < num_dimension ; i++)
        tensor->dimensions[i] = dimensions[i];

    tensor->table = NULL;

    return tensor;
}

void free_tensor(tensor_t* tensor)
{
    enlight_free((uint8_t*)tensor);
}

#define alloc_candidate(max_candidate) alloc_detection(max_candidate)
#define free_candidate(candidate) free_detection(candidate)

detection_t* alloc_detection(int max_detection)
{
    detection_t* detection;
    uint8_t* buf;

    int size = sizeof(detection_t);

    size += sizeof(box_t) * max_detection;
    size += sizeof(uint8_t) * max_detection;
    size += sizeof(uint8_t) * max_detection;

    buf = enlight_malloc(size);

    detection = (detection_t*)buf;

    buf += sizeof(detection_t);
    detection->box = (box_t*)buf;

    buf += sizeof(box_t) * max_detection;
    detection->score = buf;

    buf += sizeof(uint8_t) * max_detection;
    detection->class = buf;

    detection->n = 0;

    return detection;
}

void free_detection(detection_t* detection)
{
    enlight_free((uint8_t*)detection);
}

void init_detections(detection_t* d, int n, box_t* box, uint8_t* class, uint8_t* score)
{
    d->box = box;
    d->class = class;
    d->score = score;

    d->n = n;
}

void copy_box(box_t* dst, box_t* src)
{
    dst->x_min = src->x_min;
    dst->x_max = src->x_max;
    dst->y_min = src->y_min;
    dst->y_max = src->y_max;
}

void copy_box_f(box_f_t* dst, box_f_t* src)
{
    dst->x_min = src->x_min;
    dst->x_max = src->x_max;
    dst->y_min = src->y_min;
    dst->y_max = src->y_max;
}

void copy_tensor_info(tensor_t* dst, tensor_t* src)
{
    dst->log2_scale = src->log2_scale;
    dst->data_type = src->data_type;

    dst->num_element = src->num_element;
    dst->num_dimension = src->num_dimension;

    dst->dimensions[0] = src->dimensions[0];
    dst->dimensions[1] = src->dimensions[1];
    dst->dimensions[2] = src->dimensions[2];
    dst->dimensions[3] = src->dimensions[3];
}

tensor_t* softmax(tensor_t* src)
{
    tensor_t* dst = alloc_tensor(TYPE_UINT8, src->num_dimension, src->dimensions, 0);

    uint32_t e, sum, _sum;
    int class;

    // batches, num_boxes, num_classes
    int num_box = src->dimensions[1];
    int num_class = src->dimensions[2];

    int8_t* next;
    int8_t* last = src->buf + (num_box * TENSOR_BUF_STRIDE) - TENSOR_BUF_STRIDE;

    uint8_t* conf = (uint8_t*)dst->buf;
    uint32_t* table = (uint32_t*)src->table + 128;
    uint32_t exp_look_up[100];

    for (next = src->buf ; next <= last ; next += TENSOR_BUF_STRIDE)
    {
        sum = 0.;

        for (class = 0 ; class < num_class ; class++)
        {
            e = table[next[class]];
            exp_look_up[class] = e;
            sum += e;
        }

        _sum = (1<<31) / sum;
        for (class = 0 ; class < num_class ; class++)
        {
            uint64_t e_ = (uint64_t)exp_look_up[class];
            *conf++ = (uint8_t)MIN(((e_ * _sum) >> (31 - 8)), 255);
        }
    }

    return dst;
}

#ifdef MAKE_EXPTABLE
uint32_t (*build_exp_tables())[256]
{
    int i, log2_scale;
    double e;

    uint32_t (*table)[256];

    table = (uint32_t (*)[256])malloc(3*8*4*256);
    int min_s = -8;
    int max_s = 16;
    int len_s = max_s - min_s;

    for (log2_scale = 0; log2_scale < len_s; log2_scale++) {

        for (i = 0 ; i < 128; i++) {
            e = round(exp(i / pow(2, log2_scale + min_s)) * 256. + 0.5);
            table[log2_scale][128+i] = (uint32_t)MIN(e, 4294967295/32);
        }

        for (i = 128 ; i <  256; i++) {
            e = round(exp((i-256) / pow(2, log2_scale + min_s)) * 256. + 0.5);
            table[log2_scale][i-128] = (uint32_t)MIN(e, 4294967295/32);
        }
    }

    {
        FILE* fp;
        char filename[256];
   
        sprintf(filename, "../exp_tables.h");
        fp = fopen(filename, "w");

        fprintf(fp, "#ifndef __EXP_TABLES_H__\n");
        fprintf(fp, "#define __EXP_TABLES_H__\n");
        fprintf(fp, "\n");
        fprintf(fp, "uint32_t exp_tables[%d][256] = {\n", len_s);

        for (log2_scale=0; log2_scale < len_s; log2_scale++) {

            fprintf(fp, "    /*  %d */\n", log2_scale + min_s);
            fprintf(fp, "    {\n");

            for (i = 0 ; i < 256 ; i++)
            {
                if (i % 16 == 0)
                    fprintf(fp, "    ");
                fprintf(fp, "0x%08x, ", table[log2_scale][i]);
                if (i % 16 == 15)
                    fprintf(fp, "\n");
            }

            fprintf(fp, "    },\n");
        }

        fprintf(fp, "};\n");

        fprintf(fp, "\n");
        fprintf(fp, "#endif\n");

        fclose(fp);
    }
    return table;
}
#endif

void build_box_info(box_t* box, tensor_t* loc, tensor_t* prior, int idx)
{
    int loc_idx = idx * TENSOR_BUF_STRIDE;
    int box_idx = idx << 2;

    int8_t* _loc = (int8_t*)loc->buf + loc_idx;
    uint8_t* _prior = (uint8_t*)prior->buf + box_idx;

    uint32_t* table = (uint32_t*)loc->table;

    int x, y, w, h, x_center, y_center, x_offset, y_offset;

    const int S_cal = 8;
    const int S_tbl = 8;
    int l2s;

    // w, h scale
    // S_w = S_prior * S_tbl
    // S_h = S_prior * S_tbl
    w = (int)_prior[2] * table[_loc[2]+128];
    h = (int)_prior[3] * table[_loc[3]+128];

    l2s = (prior->log2_scale + S_tbl) - S_cal;
    if (l2s > 0) {
        w >>= l2s;
        h >>= l2s;
    }
    else {
        w <<= -l2s;
        h <<= -l2s;
    }

    // x_offset, y_offset scale
    // S_xoff = S_loc * S_prior
    x_offset = (int)_loc[0] * (int)_prior[2];
    y_offset = (int)_loc[1] * (int)_prior[3];

    l2s = (loc->log2_scale + prior->log2_scale) - S_cal;
    if (l2s > 0) {
        x_offset >>= l2s;
        y_offset >>= l2s;
    }
    else {
        x_offset <<= -l2s;
        y_offset <<= -l2s;
    }

    l2s = (prior->log2_scale) - S_cal;
    if (l2s > 0) {
        x_center = _prior[0] >> l2s;
        y_center = _prior[1] >> l2s;
    }
    else {
        x_center = _prior[0] << (-l2s);
        y_center = _prior[1] << (-l2s);
    }

    x = x_center - w / 2 + x_offset;
    y = y_center - h / 2 + y_offset;

    box->x_min = x;
    box->x_max = x + w;
    box->y_min = y;
    box->y_max = y + h;
}

// input: { batch, boxes, classes }
// output: { boxes { coordinate, score } }
candidate_t* thresholding(tensor_t* score, tensor_t* loc, tensor_t* prior,
                          int class, uint8_t th)
{
    uint8_t* _score = score->buf + class;

    int num_box = score->dimensions[1];
    int num_class = score->dimensions[2];

    int box, n;
    int prior_offset;

    candidate_t* candidate = (candidate_t*)alloc_candidate(num_box);

    for (box = 0, n = 0 ; box < num_box ; box++, _score += num_class)
    {
        if (*_score >= th)
        {
            build_box_info(&candidate->box[n], loc, prior, box);

            candidate->class[n] = class;
            candidate->score[n] = *_score;

            n++;
        }
    }

    candidate->n = n;

    return candidate;
}

int iou(box_t* l, box_t* r)
{
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
        return 255;
    else
        return 255 * area_inter / area_union;
}

void nms(detection_t* detection, candidate_t* candidate, int class, uint8_t th)
{
    int* indice = (int*)enlight_malloc(sizeof(int)*candidate->n);

    int* head;
    int* next;
    int* last;

    int* argmax;

    int max, score;

    box_t* _src_box = candidate->box;
    uint8_t* _src_score = candidate->score;

    int i, n = detection->n;
    int num_candidate = candidate->n;

    box_t* _dst_box = detection->box;
    uint8_t* _dst_class = detection->class;
    uint8_t* _dst_score = detection->score;

    for (i = 0 ; i < num_candidate ; i++)
        indice[i] = i;

    // non-maximum surpression
    last = indice + num_candidate - 1;

    for (head = indice ; head <= last ; head++)
    {
        // arg max(score)
        argmax = head;
        max = _src_score[*head];
        for (next = head + 1 ; next <= last ; next++)
        {
            score = _src_score[*next];

            if (score > max)
            {
                argmax = next;
                max = score;
            }
        }

        // swap
        swap(head, argmax);

        // add detection result
        copy_box(&_dst_box[n], &_src_box[*head]);

        _dst_class[n] = class;
        _dst_score[n] = max;

        n++;

        // non-maximum surpression
        for (next = head + 1; next <= last ; next++)
        {
            if (iou(&_src_box[*head], &_src_box[*next]) >= th)
            {
                swap(last, next);

                last--;
                next--;
            }
        }
    }

    detection->n = n;

    enlight_free((uint8_t*)indice);
}

detection_t* post_process(tensor_t* score, tensor_t* loc, tensor_t* prior,
                          uint8_t th_conf, uint8_t th_nms)
{
    detection_t* detection = alloc_detection(score->num_element);

    // batches, boxes, classes
    int num_class = score->dimensions[2];
    int class;

    tensor_t* confidence;
    candidate_t* candidate;

    // order of dimension is { batch, default_box, scores }
    confidence = softmax(score);

    for (class = 1 ; class < num_class ; class++)
    {
        candidate = thresholding(confidence, loc, prior, class, th_conf);

        nms(detection, candidate, class, th_nms);

        free_candidate(candidate);
    }

    free_tensor(confidence);

    return detection;
}

int detector(void* prior_buf, void* loc_buf, void* score_buf,
             int log2_prior_scl, int log2_loc_scl, int log2_score_scl,
             int num_class, int th_conf, int th_iou, int num_box)
{

    volatile unsigned long cycle0_h;
    volatile unsigned long cycle1_h;

    volatile unsigned long cycle0;
    volatile unsigned long cycle1;

    unsigned long hz;

    tensor_t score;
    tensor_t loc;
    tensor_t prior;

#ifdef MAKE_EXPTABLE
    uint32_t (*exp_tables)[256] = build_exp_tables();
#endif

    int i;

#ifndef SUPPORT_MALLOC
    for (i = 0 ; i < MALLOC_NUM ; i++)
        malloc_buf_used[i] = 0;
#endif

    prior.buf = prior_buf;
    prior.log2_scale = log2_prior_scl;
    prior.data_type = TYPE_UINT8;
    prior.num_element = num_box*4;
    prior.num_dimension = 2;
    prior.dimensions[0] = num_box;
    prior.dimensions[1] = 4;
    prior.table = NULL;

    loc.buf = loc_buf;
    loc.log2_scale = log2_loc_scl;
    loc.data_type = TYPE_INT8;
    loc.num_element = 1*num_box*4;
    loc.num_dimension = 3;
    loc.dimensions[0] = 1;
    loc.dimensions[1] = num_box;
    loc.dimensions[2] = 4;
    loc.table = exp_tables[8 + log2_loc_scl];

    score.buf = score_buf;
    score.log2_scale = log2_score_scl;
    score.data_type = TYPE_INT8;
    score.num_element = 1*num_box*num_class;
    score.num_dimension = 3;
    score.dimensions[0] = 1;
    score.dimensions[1] = num_box;
    score.dimensions[2] = num_class;
    score.table = exp_tables[8 + log2_score_scl];

    if ((log2_score_scl < -7) || (log2_score_scl > 15)) {
        enlight_log("exp_tables is not ready for log2_score_scl {%d}\n", log2_score_scl);
    }

    if ((log2_loc_scl < -7) || (log2_loc_scl > 15)) {
        enlight_log("exp_tables is not ready for log2_loc_scl {%d}\n", log2_loc_scl);
    }

#ifdef DETECTOR_DEBUG
    enlight_log(" prior.log2_scale   %08x\n",     prior.log2_scale);
    enlight_log(" prior.data_type    %08x\n",     prior.data_type);
    enlight_log(" prior.num_element  %08x\n",     prior.num_element);
    enlight_log(" prior.num_dimension%08x\n",     prior.num_dimension);
    enlight_log(" prior.dimensions[0]%08x\n",     prior.dimensions[0]);
    enlight_log(" prior.dimensions[1]%08x\n",     prior.dimensions[1]);
    enlight_log(" prior.table        %08x\n",     prior.table);
    enlight_log("                    %08x\n",     0);
    enlight_log(" loc.buf            %08x\n",     loc.buf );
    enlight_log(" loc.log2_scale     %08x\n",     loc.log2_scale);
    enlight_log(" loc.data_type      %08x\n",     loc.data_type);
    enlight_log(" loc.num_element    %08x\n",     loc.num_element);
    enlight_log(" loc.num_dimension  %08x\n",     loc.num_dimension);
    enlight_log(" loc.dimensions[0]  %08x\n",     loc.dimensions[0]);
    enlight_log(" loc.dimensions[1]  %08x\n",     loc.dimensions[1]);
    enlight_log(" loc.dimensions[2]  %08x\n",     loc.dimensions[2]);
    enlight_log(" loc.table          %08x\n",     loc.table);
    enlight_log("                    %08x\n",     0);
    enlight_log(" score.buf          %08x\n",     score.buf);
    enlight_log(" score.log2_scale   %08x\n",     score.log2_scale);
    enlight_log(" score.data_type    %08x\n",     score.data_type);
    enlight_log(" score.num_element  %08x\n",     score.num_element);
    enlight_log(" score.num_dimension%08x\n",     score.num_dimension);
    enlight_log(" score.dimensions[0]%08x\n",     score.dimensions[0]);
    enlight_log(" score.dimensions[1]%08x\n",     score.dimensions[1]);
    enlight_log(" score.dimensions[2]%08x\n",     score.dimensions[2]);
    enlight_log(" score.table        %08x\n",     score.table);
#endif

    enlight_log("Postproc start\n");
    hz = 600 * 1024;
    asm volatile ("rdcycle %0" : "=r" (cycle0));

    detection_t* r1 = post_process(&score, &loc, &prior, th_conf, th_iou);

    asm volatile ("rdcycle %0" : "=r" (cycle1));

    enlight_log("%d object is detected\n", r1->n);

    enlight_log("[Postproc cycles] %ld \n", cycle1 - cycle0);
    //enlight_log("%ld ms taken\n", (cycle1 - cycle0) / hz);
    
    objects.cnt=0;

    for (i = 0 ; i < r1->n ; i++)
    {
        box_t* b = r1->box + i;

        enlight_log("x_min: %d, x_max: %d, y_min: %d, y_max: %d, score: %d, class: %d\n", 
                300*b->x_min/256,
                300*b->x_max/256,
                300*b->y_min/256,
                300*b->y_max/256,
                ((int)r1->score[i])*100/256,
                r1->class[i]);


        obj_t *detect_obj = &objects.obj[objects.cnt];
        detect_obj->x_min = 300*b->x_min/256;
        detect_obj->x_max = 300*b->x_max/256;
        detect_obj->y_min = 300*b->y_min/256;
        detect_obj->y_max = 300*b->y_max/256;
        detect_obj->class = r1->score[i];
        detect_obj->score = r1->class[i];
        objects.cnt++;

    }


#ifdef MAKE_EXPTABLE
    free(exp_tables);
#endif

    return 0;
}

void detector_init(void *work_buf)
{ 
    objects.cnt=0;
#ifndef SUPPORT_MALLOC
    malloc_buf = (uint8_t*)work_buf;
#endif
}

void detector_run(
    uint8_t *prior_box,
    uint8_t *loc_buf_32,
    uint8_t *score_buf_32,
    int log2_prior_scl,
    int log2_loc_scl,
    int log2_score_scl,
    int num_class, int stages, int *grids, int* vars,
    int th_conf, int th_iou, int num_box) 
{

    log2_loc_scl += 3;

    detector(prior_box, loc_buf_32, score_buf_32,
             log2_prior_scl, log2_loc_scl, log2_score_scl,
             num_class, (uint8_t)th_conf, (uint8_t)th_iou, num_box);
}
