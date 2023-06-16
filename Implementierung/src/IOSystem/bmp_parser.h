#include <stddef.h>
#include <stdint.h>

typedef struct pixel24_t {
    uint8_t b;
    uint8_t g;
    uint8_t r;
} pixel24_t;

typedef struct uBMPImage {
    uint32_t width;
    uint32_t height;
    pixel24_t* pxArray;
} uBMPImage;

struct BMPFile {
    size_t size;
    void* buf;
};


struct BMPFile* readBMPFile (const char* path);

uBMPImage* parseBMPFile (struct BMPFile* bmpFile);