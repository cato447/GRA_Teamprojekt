#ifndef BMP_DEFINITIONS_H
#define BMP_DEFINITIONS_H

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

typedef struct uBMPImage {
    uint32_t pxWidth;
    uint32_t pxHeight;
    size_t pxArraySize;
    uint8_t *pxArray;
} uBMPImage;

#endif
