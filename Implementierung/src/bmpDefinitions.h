#include <stdint.h>

typedef struct pixel24_t {
    uint8_t b;
    uint8_t g;
    uint8_t r;
} pixel24_t;

typedef struct uBMPImage {
    uint32_t pxWidth;
    uint32_t pxHeight;
    pixel24_t* pxArray;
} uBMPImage;
