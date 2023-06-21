#include "bmp_parser.h"

#include <stdio.h>

#define BMP_HEADER_SIGN 0x4d42
#define FILESIZE_OFFS 0x02
#define DATAOFFS_OFFS 0x0a
#define PXWIDTH_OFFS 0x12
#define PXHEIGHT_OFFS 0x16
/*
Returns 0 on success and -1 on failure.
Sets "width", "height" and "pxArray" of "bmpImgBuf" to an unpadded copy of the of the parameter "bmpFile".
Pixelarray starts in bottom left of picture.
*/
int bmpToArray(const void* buf, size_t bufSize, uBMPImage* bmpImgBuf) {
    if (bufSize < 26) {
        fprintf(stderr, "Error: file too small\n");
        return -1;
    }

    if (*(uint16_t*) buf != BMP_HEADER_SIGN) {
        fprintf(stderr, "Error: incorrect file header signature\n");
        return -1;
    }

    if (*(uint32_t*)(buf + FILESIZE_OFFS) != bufSize) {
        fprintf(stderr, "Error: file size not matching size specified in file info\n");
        return -1;
    }

    int32_t pxWidth = *(int32_t*)(buf + PXWIDTH_OFFS);
    if (pxWidth < 0) {
        fprintf(stderr, "Error: image width must't be negative");
        return -1;
    }

    int negHeight = 0;
    int32_t pxHeight = *(int32_t*)(buf + PXHEIGHT_OFFS);
    if (pxHeight < 0) {
        pxHeight = -pxHeight;
        negHeight = -1;
    }

    uint32_t dataOffset = *(uint32_t*)(buf + DATAOFFS_OFFS);

    int32_t byteWidth = pxWidth * sizeof(pixel24_t);
    int32_t byteWidthPadded = (byteWidth & 0x3) ? ((byteWidth & ~0x3) + 4) : byteWidth;

    if (dataOffset + byteWidthPadded * pxHeight > bufSize) {
        fprintf(stderr, "Error: file size doesn't match file info\n");
        return -1;
    }
    
    pixel24_t* pxArray = malloc(byteWidth * pxHeight);
    if (!pxArray) {
        fprintf(stderr, "Error: failed allocating memory for pixel array\n");
        return -1;
    }
    
    pixel24_t* pxArrayEnd = pxArray + pxWidth * pxHeight;
    if (negHeight) {
        for (pixel24_t* dest = pxArrayEnd - pxWidth; dest >= pxArray; dest -= pxWidth, buf += byteWidthPadded) {
            memcpy(dest, buf + dataOffset, byteWidth);
        }
    } else {
        for (pixel24_t* dest = pxArray; dest < pxArrayEnd; dest += pxWidth, buf += byteWidthPadded) {
            memcpy(dest, buf + dataOffset, byteWidth);
        }
    }

    bmpImgBuf->pxArray = pxArray;
    bmpImgBuf->pxWidth = pxWidth;
    bmpImgBuf->pxHeight = pxHeight;
    return 0;
}

void* arrayToBmp(const uBMPImage *bmpImgBuf, size_t *bufSize) {
    return 0;
}
