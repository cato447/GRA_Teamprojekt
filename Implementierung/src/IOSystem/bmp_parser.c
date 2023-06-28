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

    // We allocate additional memory for an extra frame of black pixels to simplify further optimizations

    size_t pxArraySize = (pxWidth + 2) * (pxHeight + 2);
    pixel24_t* pxArray = calloc(pxArraySize, sizeof(pixel24_t));
    if (!pxArray) {
        fprintf(stderr, "Error: failed allocating memory for pixel array\n");
        return 1;
    }
    //+ 2 because of black frame
    pixel24_t* pxArrayEnd = pxArray + (pxWidth + 2) * (pxHeight + 2);
    char* bufInc = buf + dataOffset;

    if (negHeight) {
        for (pixel24_t* dest = pxArrayEnd - (pxWidth - 1); dest >= (pxArray - pxWidth - 2); dest -= pxWidth + 2, bufInc += byteWidthPadded) {
            memcpy(dest, bufInc, byteWidth);
        }
    } else {
        for (pixel24_t* dest = pxArray + pxWidth + 2 + 1; dest < pxArrayEnd - pxWidth - 2 - 1; dest += pxWidth + 2, bufInc += byteWidthPadded) {
            memcpy(dest, bufInc, byteWidth);
        }
    }

    bmpImgBuf->pxArray = pxArray;
    bmpImgBuf->pxWidth = pxWidth+2;
    bmpImgBuf->pxHeight = pxHeight+2;
    bmpImgBuf->pxArraySize = pxArraySize;

    return 0;
}

char* arrayToBmp(const uBMPImage* bmpImg, size_t* size) {
    uint32_t byteWidth = bmpImg->pxWidth * sizeof(pixel24_t) - 2 * sizeof(pixel24_t);
    uint32_t byteWidthPadded = (byteWidth & 0x3) ? ((byteWidth & ~0x3) + 4) : byteWidth;

    *size = HEADER_SIZE + INFO_HEADER_SIZE + byteWidthPadded * (bmpImg->pxHeight - 2);
    char* buf = calloc(*size, 1);

    *(uint16_t*)(buf + 0x0) = BMP_HEADER_SIGN;
    *(uint32_t*)(buf + 0x2) = *size;
    *(uint32_t*)(buf + 0xA) = HEADER_SIZE + INFO_HEADER_SIZE;
    *(uint32_t*)(buf + 0xE) = INFO_HEADER_SIZE;
    *(uint32_t*)(buf + 0x12) = bmpImg->pxWidth - 2;
    *(uint32_t*)(buf + 0x16) = bmpImg->pxHeight - 2;
    *(uint16_t*)(buf + 0x1A) = 1;
    *(uint16_t*)(buf + 0x1C) = 24;
    *(uint32_t*)(buf + 0x26) = 3800;
    *(uint32_t*)(buf + 0x2A) = 3800;
    *(uint32_t*)(buf + 0x2E) = 256 * 256 * 256;

    char* pxData = buf + HEADER_SIZE + INFO_HEADER_SIZE;
    pixel24_t* pxArraySrc = bmpImg->pxArray + bmpImg->pxWidth;
    pixel24_t* pxArrayEnd = bmpImg->pxArray + bmpImg->pxArraySize - bmpImg->pxWidth;

    while (pxArraySrc < pxArrayEnd) {
        memcpy(pxData, pxArraySrc + 1, byteWidth);
        pxArraySrc += bmpImg->pxWidth;
        pxData += byteWidthPadded;
    }
    
    return buf;
}
