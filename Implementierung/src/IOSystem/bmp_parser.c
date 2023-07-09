#include "bmp_parser.h"

#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

#define BMP_HEADER_SIGN 0x4d42
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40

struct __attribute__((__packed__)) bmpHeader {
    uint16_t signature;
    uint32_t fileSize;
    uint32_t _zero0;
    uint32_t dataOffset;
    uint32_t infoHeadersize;
    int32_t pxWidth;
    int32_t pxHeight;
    uint16_t planes;
    uint16_t bitDepth;
    uint32_t _zero1;
    uint32_t _zero2;
    int32_t pxPerMx;
    int32_t pxPerMy;
    uint32_t numColors;
    uint32_t _zero3;
};

/*
Returns 0 on success and -1 on failure.
Sets "width", "height" and "pxArray" of "bmpImgBuf" to an unpadded copy of the of the parameter "bmpFile".
Pixelarray starts in bottom left of picture.
*/
int bmpToArray(char* buf, size_t bufSize, uBMPImage* bmpImgBuf) {
    if (bufSize < 26) {
        fprintf(stderr, "Error: file too small\n");
        return 1;
    }

    struct bmpHeader* header = (struct bmpHeader*) buf;

    if (header->signature != BMP_HEADER_SIGN) {
        fprintf(stderr, "Error: Input isn't a BMP file\n");
        return 1;
    }

    if (header->fileSize != bufSize) {
        fprintf(stderr, "Error: file size not matching size specified in file info\n"); 
        return 1;
    }

    int32_t pxWidth = header->pxWidth;
    if (pxWidth < 0) {
        fprintf(stderr, "Error: image width can't be negative");
        return 1;
    }

    int negHeight = 0;
    int32_t pxHeight = header->pxHeight;
    if (pxHeight < 0) {
        pxHeight = -pxHeight;
        negHeight = 1;
    }

    uint32_t dataOffset = header->dataOffset;

    int32_t byteWidth = pxWidth * sizeof(pixel24_t);
    int32_t byteWidthPadded = (byteWidth & 0x3) ? ((byteWidth & ~0x3)) : byteWidth;

    if (dataOffset + byteWidthPadded * pxHeight > bufSize) {
        fprintf(stderr, "Error: file size doesn't match file info\n");
        return 1;
    }


    size_t pxArraySize = pxWidth * pxHeight;
    pixel24_t* pxArray = calloc(pxArraySize, sizeof(pixel24_t));
    if (!pxArray) {
        fprintf(stderr, "Error: failed allocating memory for pixel array\n");
        return 1;
    }

    pixel24_t* pxArrayEnd = pxArray + pxWidth * pxHeight;
    char* bufInc = buf + dataOffset;

    if (negHeight) {
        for (pixel24_t* dest = pxArrayEnd - pxWidth; dest >= pxArray; dest -= pxWidth, bufInc += byteWidthPadded) {
            memcpy(dest, bufInc, byteWidth);
        }
    } else {
        for (pixel24_t* dest = pxArray; dest < pxArrayEnd; dest += pxWidth, bufInc += byteWidthPadded) {
            memcpy(dest, bufInc, byteWidth);
        }
    }

    bmpImgBuf->pxArray = pxArray;
    bmpImgBuf->pxWidth = pxWidth;
    bmpImgBuf->pxHeight = pxHeight;
    bmpImgBuf->pxArraySize = pxArraySize;

    return 0;
}


#define NUM_PLANES 1
#define PX_PER_METER 3800
#define BIT_DEPTH 3 * 8
#define NUM_COLORS 0 

const struct bmpHeader headerTemplate = {
    .signature = BMP_HEADER_SIGN,
    .dataOffset = HEADER_SIZE + INFO_HEADER_SIZE,
    .infoHeadersize = INFO_HEADER_SIZE,
    .planes = NUM_PLANES,
    .bitDepth = BIT_DEPTH,
    .pxPerMx = PX_PER_METER,
    .pxPerMy = PX_PER_METER,
    .numColors = NUM_COLORS,
    ._zero0 = 0,
    ._zero1 = 0,
    ._zero2 = 0,
    ._zero3 = 0};

static inline void generateHeaderInfoHeader(struct bmpHeader* buf, uint32_t fileSize, int32_t width, int32_t height) {
    *buf = headerTemplate;
    buf->fileSize = fileSize;
    buf->pxWidth = width;
    buf->pxHeight = height;
}

/*
Return a pointer to a buffer containing a complete BMP image.
Writes size of buffer to parameter "size".
*/
char* arrayToBmp(const uBMPImage* bmpImg, size_t* size) {
    uint32_t byteWidth = bmpImg->pxWidth * sizeof(pixel24_t);

    *size = HEADER_SIZE + INFO_HEADER_SIZE + byteWidth * (bmpImg->pxHeight);
    char* buf = calloc(*size, sizeof(uint8_t));

    generateHeaderInfoHeader((struct bmpHeader*) buf, *size, bmpImg->pxWidth, bmpImg->pxHeight);

    char* pxData = buf + HEADER_SIZE + INFO_HEADER_SIZE;
    pixel24_t* pxArraySrc = bmpImg->pxArray;
    pixel24_t* pxArrayEnd = bmpImg->pxArray + bmpImg->pxArraySize;

    while (pxArraySrc < pxArrayEnd) {
        memcpy(pxData, pxArraySrc, byteWidth);
        pxArraySrc += bmpImg->pxWidth;
        pxData += byteWidth;
    }
    
    return buf;
}
