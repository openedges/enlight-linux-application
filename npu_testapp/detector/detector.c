#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#define DETECTOR_DEBUG

#ifdef TEST_ON_PC
#define SUPPORT_MALLOC
#define MAKE_EXPTABLE
#endif

#ifndef TEST_ON_PC
#   include "exp_tables.h"
#endif

/*
#ifdef TEST_ON_PC
#define SUPPORT_MALLOC

//#   include "prior_box.h"
#endif
*/

#ifdef TEST_ON_PC
#   define __i386__
#endif

#undef __i86__
#ifdef __i386__
#   define enlight_log(...)  do { printf(__VA_ARGS__);  } while(0)
#else
#   define enlight_log(...)  do { printf(__VA_ARGS__); } while(0)
#endif

//#define NUM_BOX 8732
#define NUM_BOX 3000
//#define NUM_CLASS 21

//#define TH_IOU  192
//#define TH_IOU  128
//#define TH_IOU  96
#define TH_IOU  96
//#define TH_CONF 192
//#define TH_CONF 128
//#define TH_CONF 96
#define TH_CONF 64

#define NUM_BIT (8 - 1)

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

#define MALLOC_UNIT (256*1024)
#define MALLOC_NUM (20)

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

            return (uint8_t*)(ptr + 2);
        }
    }

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

    uint32_t e, sum;
    int class;

    // batches, num_boxes, num_classes
    int num_box = src->dimensions[1];
    int num_class = src->dimensions[2];

    int8_t* next;
    int8_t* last = src->buf + (num_box * num_class) - num_class;

    uint8_t* conf = (uint8_t*)dst->buf;
    uint32_t* table = (uint32_t*)src->table;

    for (next = src->buf ; next <= last ; next += num_class)
    {
        sum = 0.;

        for (class = 0 ; class < num_class ; class++)
        {
            e = table[next[class]+128];
            sum += e;
        }

        for (class = 0 ; class < num_class ; class++)
        {
            e = table[next[class]+128];
            *conf++ = (uint8_t)(MIN(256 * e / sum, 255));
        }
    }

#if 0
    conf = (uint8_t*)dst->buf;

    for (next = src->buf ; next <= last ; next += num_class)
    {
        printf("\n");

        for (class = 0 ; class < num_class ; class++)
            printf("%f ", *conf++/256.);
        
    }
#endif

    return dst;
}

#ifdef TEST_ON_PC
uint32_t (*build_exp_tables())[256]
{
    int i, log2_scale;
    double e;

    uint32_t (*table)[256];

    table = (uint32_t (*)[256])malloc(8*4*256);

    for (log2_scale = 0; log2_scale < 8; log2_scale++) {

        for (i = 0 ; i < 128; i++) {
            e = round(exp(i / pow(2, (NUM_BIT - log2_scale))) * 256. + 0.5);
            table[log2_scale][128+i] = (uint32_t)MIN(e, 4294967295/32);
        }

        for (i = 128 ; i <  256; i++) {
            e = round(exp((i-256) / pow(2, (NUM_BIT - log2_scale))) * 256. + 0.5);
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
        fprintf(fp, "uint32_t exp_tables[8][256] = {\n");

        for (log2_scale=0; log2_scale < 8; log2_scale++) {

            fprintf(fp, "    /*  %d */\n", log2_scale);
            fprintf(fp, "    {\n");

            for (i = 0 ; i < 256 ; i++)
            {
                if (i % 16 == 0)
                    fprintf(fp, "    ");
                fprintf(fp, "0x%08x, ", table[log2_scale][i]);
                //fprintf(fp, "%8d, ", table[log2_scale][i]);
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
    int _idx = idx << 2;

    int8_t* _loc = (int8_t*)loc->buf + _idx;
    uint8_t* _prior = (uint8_t*)prior->buf + _idx;

    int _log2_scale;
    uint32_t* table = (uint32_t*)loc->table;

    int w = (int)_prior[2] * table[_loc[2]+128];
    int h = (int)_prior[3] * table[_loc[3]+128];

    // log2_scale of exponent is 0
    _log2_scale = prior->log2_scale - 8;
    if (_log2_scale > 0)
    {
        w <<= _log2_scale;
        h <<= _log2_scale;
    }
    else
    {
        w >>= -_log2_scale;
        h >>= -_log2_scale;
    }

    int x_offset = (int)_loc[0] * (int)_prior[2];
    int y_offset = (int)_loc[1] * (int)_prior[3];

    // location is int8_t and prior is uint8_t
    _log2_scale = loc->log2_scale + prior->log2_scale - 7;
    if (_log2_scale > 0)
    {
        x_offset <<= _log2_scale;
        y_offset <<= _log2_scale;
    }
    else
    {
        x_offset >>= -_log2_scale;
        y_offset >>= -_log2_scale;
    }

    int x = _prior[0] - w / 2 + x_offset;
    int y = _prior[1] - h / 2 + y_offset;

    box->x_min = x;
    box->x_max = x + w;
    box->y_min = y;
    box->y_max = y + h;
}

// input: { batch, boxes, classes }
// output: { boxes { coordinate, score } }
candidate_t* thresholding(tensor_t* score, tensor_t* loc, tensor_t* prior, int class, uint8_t th)
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

//FIXME. Merge with weight.
void matmul_var(uint8_t* tensor)
{
#define VAR0 0.4
#define VAR1 0.4
#define VAR2 0.8
#define VAR3 0.8
    int i;
    int8_t* src =(int8_t *)tensor;
    int32_t l0, l1, l2, l3;

    for (i = 0; i < NUM_BOX; i++) {
        l0 = src[i*4 + 0] * VAR0;
        l1 = src[i*4 + 1] * VAR1;
        l2 = src[i*4 + 2] * VAR2;
        l3 = src[i*4 + 3] * VAR3;

        src[i*4 + 0] = l0;
        src[i*4 + 1] = l1;
        src[i*4 + 2] = l2;
        src[i*4 + 3] = l3;
        // printf("%02x%02x%02x%02x\n",
        //     (uint8_t)src[i*4 + 0],
        //     (uint8_t)src[i*4 + 1],
        //     (uint8_t)src[i*4 + 2],
        //     (uint8_t)src[i*4 + 3]);
    }
}

void arrange_tensor(uint8_t* src, uint8_t* dst, int num_ch) 
{
    int i, r, s, h, w, c;
    const int ratio = 6;

    //[6,19,19,32] [6,10,10,32] [6,5,5,32], [6,3,3,32], [6,2,2,32], [6,1,1,32]
    const int H0 = 19, W0 = 19;
    const int H1 = 10, W1 = 10;
    const int H2 = 5,  W2 = 5;
    const int H3 = 3,  W3 = 3;
    const int H4 = 2,  W4 = 2;
    const int H5 = 1,  W5 = 1;
    const int CH = 32;
    uint8_t (*s0)[H0][W0][CH];
    uint8_t (*s1)[H1][W1][CH];
    uint8_t (*s2)[H2][W2][CH];
    uint8_t (*s3)[H3][W3][CH];
    uint8_t (*s4)[H4][W4][CH];
    uint8_t (*s5)[H5][W5][CH];

    s0 = (uint8_t (*)[H0][W0][CH])(&src[0]);
    s1 = (uint8_t (*)[H0][W0][CH])(&src[19*19*32*ratio]);
    s2 = (uint8_t (*)[H0][W0][CH])(&src[19*19*32*ratio + 10*10*32*ratio]);
    s3 = (uint8_t (*)[H0][W0][CH])(&src[19*19*32*ratio + 10*10*32*ratio + 5*5*32*ratio]);
    s4 = (uint8_t (*)[H0][W0][CH])(&src[19*19*32*ratio + 10*10*32*ratio + 5*5*32*ratio + 3*3*32*ratio]);
    s5 = (uint8_t (*)[H0][W0][CH])(&src[19*19*32*ratio + 10*10*32*ratio + 5*5*32*ratio + 3*3*32*ratio + 2*2*32*ratio]);

    for (i = 0; i < H0 * W0; i++) {
        for (r = 0; r < ratio; r++) {
            h = i / W0; w = i % W0;
            for (c = 0; c < num_ch; c++) {
                *dst++ = s0[r][h][w][c];
            }
        }
    }
    for (i = 0; i < H1 * W1; i++) {
        for (r = 0; r < ratio; r++) {
            h = i / W1; w = i % W1;
            for (c = 0; c < num_ch; c++)
                *dst++ = s1[r][h][w][c];
        }
    }
    for (i = 0; i < H2 * W2; i++) {
        for (r = 0; r < ratio; r++) {
            h = i / W2; w = i % W2;
            for (c = 0; c < num_ch; c++)
                *dst++ = s2[r][h][w][c];
        }
    }
    for (i = 0; i < H3 * W3; i++) {
        for (r = 0; r < ratio; r++) {
            h = i / W3; w = i % W3;
            for (c = 0; c < num_ch; c++)
                *dst++ = s3[r][h][w][c];
        }
    }
    for (i = 0; i < H4 * W4; i++) {
        for (r = 0; r < ratio; r++) {
            h = i / W4; w = i % W4;
            for (c = 0; c < num_ch; c++)
                *dst++ = s4[r][h][w][c];
        }
    }
    for (i = 0; i < H5 * W5; i++) {
        for (r = 0; r < ratio; r++) {
            h = i / W5; w = i % W5;
            for (c = 0; c < num_ch; c++)
                *dst++ = s5[r][h][w][c];
        }
    }
}

detection_t* post_process(tensor_t* score, tensor_t* loc, tensor_t* prior, uint8_t th_conf, uint8_t th_nms)
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
             int log2_scl_prior, int log2_scl_loc, int log2_score_scl,
             int num_class)
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

#ifdef DETECTOR_DEBUG
    {
        int i;    
        uint8_t*data = (uint8_t*)score_buf;

        for (i = 0; i < 40; i++)
            enlight_log("S[%02x]:%02x%02x%02x%02x_%02x%02x%02x%02x_%02x%02x%02x%02x_%02x%02x%02x%02x\n",
                 &score_buf[i*16],
                 data[i*16 + 0], data[i*16 + 1], data[i*16 + 2], data[i*16 + 3],
                 data[i*16 + 4], data[i*16 + 5], data[i*16 + 6], data[i*16 + 7],
                 data[i*16 + 8], data[i*16 + 9], data[i*16 + 10], data[i*16 + 11],
                 data[i*16 + 12], data[i*16 + 13], data[i*16 + 14], data[i*16 + 15]);

        data = (uint8_t*)loc_buf;

        enlight_log("\n\n");
        for (i = 0; i < 40; i++) 
            enlight_log("L[%02x]:%02x%02x%02x%02x_%02x%02x%02x%02x_%02x%02x%02x%02x_%02x%02x%02x%02x\n",
                 &loc_buf[i*16],
                 data[i*16 + 0], data[i*16 + 1], data[i*16 + 2], data[i*16 + 3],
                 data[i*16 + 4], data[i*16 + 5], data[i*16 + 6], data[i*16 + 7],
                 data[i*16 + 8], data[i*16 + 9], data[i*16 + 10], data[i*16 + 11],
                 data[i*16 + 12], data[i*16 + 13], data[i*16 + 14], data[i*16 + 15]);

        data = (uint8_t*)prior_buf;

        enlight_log("\n\n");
        for (i = 0; i < 40; i++) 
            enlight_log("P[%02x]:%02x%02x%02x%02x_%02x%02x%02x%02x_%02x%02x%02x%02x_%02x%02x%02x%02x\n",
                 &loc_buf[i*16],
                 data[i*16 + 0], data[i*16 + 1], data[i*16 + 2], data[i*16 + 3],
                 data[i*16 + 4], data[i*16 + 5], data[i*16 + 6], data[i*16 + 7],
                 data[i*16 + 8], data[i*16 + 9], data[i*16 + 10], data[i*16 + 11],
                 data[i*16 + 12], data[i*16 + 13], data[i*16 + 14], data[i*16 + 15]);
    }
#endif

    prior.buf = prior_buf;
    prior.log2_scale = log2_scl_prior;
    prior.data_type = TYPE_UINT8;
    prior.num_element = NUM_BOX*4;
    prior.num_dimension = 2;
    prior.dimensions[0] = NUM_BOX;
    prior.dimensions[1] = 4;
    prior.table = NULL;

    loc.buf = loc_buf;
    loc.log2_scale = log2_scl_loc;
    loc.data_type = TYPE_INT8;
    loc.num_element = 1*NUM_BOX*4;
    loc.num_dimension = 3;
    loc.dimensions[0] = 1;
    loc.dimensions[1] = NUM_BOX;
    loc.dimensions[2] = 4;
    loc.table = exp_tables[log2_scl_loc];

    score.buf = score_buf;
    score.log2_scale = log2_score_scl;
    score.data_type = TYPE_INT8;
    score.num_element = 1*NUM_BOX*num_class;
    score.num_dimension = 3;
    score.dimensions[0] = 1;
    score.dimensions[1] = NUM_BOX;
    score.dimensions[2] = num_class;
    score.table = exp_tables[log2_score_scl];

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
#ifdef __i386__
    hz = 36 * 1024 * 1024 / 10;
    asm volatile ("rdtsc" : "=a"(cycle0), "=d"(cycle0_h));
#else
    hz = 600 * 1024;
    asm volatile ("rdcycle %0" : "=r" (cycle0));
#endif

    detection_t* r1 = post_process(&score, &loc, &prior, TH_CONF, TH_IOU);

#ifdef __i386__
    hz = 36 * 1024 * 1024 / 10;
    asm volatile ("rdtsc" : "=a"(cycle1), "=d"(cycle1_h));
#else
    hz = 600 * 1024;
    asm volatile ("rdcycle %0" : "=r" (cycle1));
#endif

    enlight_log("%d object is detected\n", r1->n);

    enlight_log("[Postproc cycles] %ld \n", cycle1 - cycle0);
    //enlight_log("%ld ms taken\n", (cycle1 - cycle0) / hz);
    
    for (i = 0 ; i < r1->n ; i++)
    {
        box_t* b = r1->box + i;

        enlight_log("x_min: %d, x_max: %d, y_min: %d, y_max: %d, score: %d, class: %d\n", 
                300*b->x_min/256,
                300*b->x_max/256,
                300*b->y_min/256,
                300*b->y_max/256,
                r1->score[i],
                r1->class[i]);
    }

#ifdef TEST_ON_PC
    enlight_log("in floating point unit...\n");

    for (i = 0 ; i < r1->n ; i++)
    {
        box_t* b = r1->box + i;

   
        enlight_log("x_min: %.2f, x_max: %.2f, y_min: %.2f, y_max: %.2f, score: %.2f, class: %d\n", 
                b->x_min / 256.0,
                b->x_max / 256.0,
                b->y_min / 256.0,
                b->y_max / 256.0,
                r1->score[i] / 256.0,
                r1->class[i]);
    }
#endif

#ifdef MAKE_EXPTABLE
    free(exp_tables);
#endif

    return 0;
}


#ifndef SUPPORT_MALLOC
void detector_init(void *work_buf)
{ 
    malloc_buf = (uint8_t*)work_buf;
}
#endif

void detector_run(
    uint8_t *prior_box,
    uint8_t *loc_buf_32,
    uint8_t *score_buf_32,
    int log2_prior_scl,
    int log2_loc_scl,
    int log2_score_scl,
    int num_class) 
{
#ifdef SUPPORT_MALLOC
    void* loc_buf = (void*)malloc(sizeof(uint8_t)*1*NUM_BOX*4);
    void* score_buf = (void*)malloc(sizeof(uint8_t)*1*NUM_BOX*32);
#else
    uint8_t *loc_buf = malloc_buf + 20*256*1024; // 12000 = NUM_BOX * 4 = 3000 * 4
    uint8_t *score_buf = loc_buf + 16*1024;     //  63000 = NUM_BOX * 21 = 3000 * 21
#endif
    //enlight_log("0x%08x\n", loc_buf);
    //enlight_log("0x%08x\n", score_buf);

#ifdef DETECTOR_DEBUG
    {
        int i;
        uint8_t*data = score_buf_32;

        for (i = 0; i < 80; i++)
            enlight_log("[%02x]:%02x%02x%02x%02x_%02x%02x%02x%02x_%02x%02x%02x%02x_%02x%02x%02x%02x\n",
                 score_buf_32 + i*16,
                 *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, 
                 *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++);

        data = loc_buf_32;
        for (i = 0; i < 80; i++)
            enlight_log("[%02x]:%02x%02x%02x%02x_%02x%02x%02x%02x_%02x%02x%02x%02x_%02x%02x%02x%02x\n",
                 loc_buf_32 + i*16,
                 *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, 
                 *data++, *data++, *data++, *data++, 
                 *data++, *data++, *data++, *data++);
    }
#endif

    // [6, H, W, 32] -> [H, W, 6*21]
    arrange_tensor(score_buf_32, score_buf, 21);
    // [6, H, W, 4] -> [H, W, 6*4]
    arrange_tensor(loc_buf_32, loc_buf, 4);
    // loc * [0.1, 0.1, 0.2, 0.2] * (4)
    matmul_var(loc_buf);
    log2_loc_scl = log2_loc_scl - 2; //multiplied by 4 at matmul_var

    detector(prior_box, loc_buf, score_buf, log2_prior_scl , log2_loc_scl , log2_score_scl, num_class);
}

