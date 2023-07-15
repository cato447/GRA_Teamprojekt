#ifndef _BMP_DEFINITIONS_H_
#define _BMP_DEFINITIONS_H_

#include <stdint.h>
#include <stddef.h>

typedef struct pixel24_t {
    uint8_t b;
    uint8_t g;
    uint8_t r;
} pixel24_t;

typedef struct pixel8_t {
    uint8_t v;
} pixel8_t;

typedef struct s_image {
    uint32_t px_width;
    uint32_t px_height;
    size_t px_array_size;
    uint8_t *px_array;
} s_image;

#endif
