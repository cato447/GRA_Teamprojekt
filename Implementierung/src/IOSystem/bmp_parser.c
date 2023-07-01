#include "bmp_parser.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

#define BMP_HEADER_SIGN 0x4d42
#define FILESIZE_OFFS 0x02
#define DATAOFFS_OFFS 0x0a
#define PXWIDTH_OFFS 0x12
#define PXHEIGHT_OFFS 0x16
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40

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

    if (*(uint16_t*) buf != BMP_HEADER_SIGN) {
        fprintf(stderr, "Error: incorrect file header signature\n");
        return 1;
    }

    if (*(uint32_t*)(buf + FILESIZE_OFFS) != bufSize) {
        fprintf(stderr, "Error: file size not matching size specified in file info\n"); 
        return 1;
    }

    int32_t pxWidth = *(int32_t*)(buf + PXWIDTH_OFFS);
    if (pxWidth < 0) {
        fprintf(stderr, "Error: image width can't be negative");
        return 1;
    }

    int negHeight = 0;
    int32_t pxHeight = *(int32_t*)(buf + PXHEIGHT_OFFS);
    if (pxHeight < 0) {
        pxHeight = -pxHeight;
        negHeight = 1;
    }

    uint32_t dataOffset = *(uint32_t*)(buf + DATAOFFS_OFFS);

    int32_t byteWidth = pxWidth * sizeof(pixel24_t);
    int32_t byteWidthPadded = (byteWidth & 0x3) ? ((byteWidth & ~0x3) + 4) : byteWidth;

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


//const char headerInfoHeaderTemplate[HEADER_SIZE + INFO_HEADER_SIZE] = {0x42, 0x4D, 0, 0, 0, 0, 0, 0, 0, 0, INFO_HEADER_SIZE + HEADER_SIZE, 0x0, 0x0, 0x0, INFO_HEADER_SIZE, 0x0, 0x0, 0x0, 0, 0, 0, 0, 0, 0, 0, 0, 0x1, 0x0, }
#define NUM_PLANES 1
#define PX_PER_METER 3800
#define BIT_DEPTH 3 * 8
#define NUM_COLORS 256 * 256 * 256

static inline void generateHeaderInfoHeader(char* zeroBuf, uint32_t fileSize, int32_t width, int32_t height) {
    *(uint16_t*)(zeroBuf + 0x00) = BMP_HEADER_SIGN;
    *(uint32_t*)(zeroBuf + 0x02) = fileSize;
    *(uint32_t*)(zeroBuf + 0x0A) = HEADER_SIZE + INFO_HEADER_SIZE;
    *(uint32_t*)(zeroBuf + 0x0E) = INFO_HEADER_SIZE;
    *(uint32_t*)(zeroBuf + 0x12) = width;
    *(uint32_t*)(zeroBuf + 0x16) = height;
    *(uint16_t*)(zeroBuf + 0x1A) = NUM_PLANES;
    *(uint16_t*)(zeroBuf + 0x1C) = BIT_DEPTH;
    *(uint32_t*)(zeroBuf + 0x26) = PX_PER_METER;
    *(uint32_t*)(zeroBuf + 0x2A) = PX_PER_METER;
    *(uint32_t*)(zeroBuf + 0x2E) = NUM_COLORS;
}

/*
Return a pointer to a buffer containing a complete BMP image.
Writes size of buffer to parameter "size".
*/
char* arrayToBmp(const uBMPImage* bmpImg, size_t* size) {
    uint32_t byteWidth = bmpImg->pxWidth * sizeof(pixel24_t);
    uint32_t byteWidthPadded = (byteWidth & 0x3) ? ((byteWidth & ~0x3) + 4) : byteWidth;

    *size = HEADER_SIZE + INFO_HEADER_SIZE + byteWidthPadded * (bmpImg->pxHeight);
    char* buf = calloc(*size, 1);

    generateHeaderInfoHeader(buf, *size, bmpImg->pxWidth, bmpImg->pxHeight);

    char* pxData = buf + HEADER_SIZE + INFO_HEADER_SIZE;
    pixel24_t* pxArraySrc = bmpImg->pxArray + bmpImg->pxWidth;
    pixel24_t* pxArrayEnd = bmpImg->pxArray + bmpImg->pxArraySize - bmpImg->pxWidth;

    while (pxArraySrc < pxArrayEnd) {
        memcpy(pxData, pxArraySrc, byteWidth);
        pxArraySrc += bmpImg->pxWidth;
        pxData += byteWidthPadded;
    }
    
    return buf;
}
