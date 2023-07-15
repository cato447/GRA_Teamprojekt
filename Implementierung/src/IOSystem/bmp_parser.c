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


//INPUT STUFF

struct __attribute__((__packed__)) parseBMPHeader {
    uint16_t signature;
    uint32_t fileSize;
    uint32_t _0;
    uint32_t dataOffset;
    uint32_t _1;
    int32_t pxWidth;
    int32_t pxHeight;
};

char *parseHeader(char *buf, size_t bufSize, size_t *_pxDataWidth, uint32_t *_pxWidth, uint32_t *_pxHeight, int *_negHeight) {
    if (bufSize < sizeof(struct parseBMPHeader)) {
        fprintf(stderr, "Error: file too small to contain relevant BMP header information\n");
        return NULL;
    }

    struct parseBMPHeader *header = (struct parseBMPHeader *) buf;

    if (header->signature != BMP_HEADER_SIGN) {
        fprintf(stderr, "Error: Input not a BMP file (header mismatch)\n");
        fprintf(stderr, "  Expected [0x4d42], got [0x%x]\n", header->signature);
        return NULL;
    }

    if (header->fileSize != bufSize) {
        printf("Warning: size specified in file header not matching real file size, file may be corrupted\n");
        printf("  Expected: %uB, got %luB\n", header->fileSize, bufSize);
    }

    if (header->pxWidth < 0) {
        printf("Warning: negative height value in file header (%d), using absolute value (%d)\n", header->pxWidth, -header->pxWidth);
        *_pxWidth = -header->pxWidth;
    } else {
        *_pxWidth = header->pxWidth;
    }

    if (header->pxHeight < 0) {
        *_pxHeight = -header->pxHeight;
        *_negHeight = 1;
    } else {
        *_pxHeight = header->pxHeight;
        *_negHeight = 0;
    }

    size_t byteWidth = header->pxWidth * sizeof(pixel24_t);
    *_pxDataWidth = (byteWidth & 0x3) ? ((byteWidth & ~0x3) + 4) : byteWidth;

    if ((header->dataOffset + *_pxDataWidth * (*_pxHeight)) > bufSize) {
        fprintf(stderr, "Error: file size doesn't match file info\n");
        fprintf(stderr, "  Expected (at max) %luB, got %luB\n", bufSize, header->dataOffset + *_pxDataWidth * (*_pxHeight));
        return NULL;
    }

    return buf + header->dataOffset;
}

/*
Returns 0 on success and 1 on failure.
Sets "width", "height", "pxArraySize" and "pxArray" of "_bmpImgBuf" to an unpadded copy of the of the parameter "bmpFile".
Pixelarray starts in bottom left of picture.
*/
int bmpToArray(char *buf, size_t bufSize, uBMPImage *_bmpImgBuf) {
    size_t pxDataWidth;
    uint32_t pxWidth;
    uint32_t pxHeight;
    int negHeight;
    char *pxData = parseHeader(buf, bufSize, &pxDataWidth, &pxWidth, &pxHeight, &negHeight);
    if (pxData == NULL) {
        fprintf(stderr, " → Failed parsing the file header\n");
        return 1;
    }

    size_t pxArrayWidth = pxWidth * sizeof(pixel24_t);

    size_t pxArraySize = pxArrayWidth * pxHeight;
    uint8_t *pxArray = calloc(pxArraySize, sizeof(uint8_t));
    if (pxArray == NULL) {
        fprintf(stderr, "Failed allocating memory for pxArray\n");
        return 1;
    }

    uint8_t *pxArrayEnd = pxArray + pxArrayWidth * pxHeight;

    if (negHeight) {
        for (uint8_t *dest = pxArrayEnd - pxArrayWidth; dest >= pxArray; dest -= pxArrayWidth, pxData += pxDataWidth) {
            memcpy(dest, pxData, pxArrayWidth);
        }
    } else {
        for (uint8_t *dest = pxArray; dest < pxArrayEnd; dest += pxArrayWidth, pxData += pxDataWidth) {
            memcpy(dest, pxData, pxArrayWidth);
        }
    }

    _bmpImgBuf->pxArray = pxArray;
    _bmpImgBuf->pxWidth = pxWidth;
    _bmpImgBuf->pxHeight = pxHeight;
    _bmpImgBuf->pxArraySize = pxArraySize;

    return 0;
}

//Scaling factors: https://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.709-6-201506-I!!PDF-E.pdf
#define R_SCALING .2126f
#define G_SCALING .7152f
#define B_SCALING .0722f
/*
Returns 0 on success and 1 on failure.
Sets "width", "height" and "pxArray" of "_bmpImgBuf" to an unpadded copy of the of the parameter "bmpFile".
Pixelarray starts in bottom left of picture.
*/
int bmpToArray_graysc (char *buf, size_t bufSize, uBMPImage *_bmpImgBuf) {
    size_t pxDataWidth;
    uint32_t pxWidth;
    uint32_t pxHeight;
    int negHeight;
    char *pxData = parseHeader(buf, bufSize, &pxDataWidth, &pxWidth, &pxHeight, &negHeight);
    if (pxData == NULL) {
        fprintf(stderr, " → Failed parsing the file header\n");
        return 1;
    }

    size_t pxArrayWidth = pxWidth * sizeof(pixel8_t);

    size_t pxArraySize = pxArrayWidth * pxHeight;
    uint8_t *pxArray = calloc(pxArraySize, sizeof(uint8_t));
    if (pxArray == NULL) {
        fprintf(stderr, "Failed allocating memory for pxArray\n");
        return 1;
    }

    uint8_t *pxArrayEnd = pxArray + pxArrayWidth * pxHeight;

    if (negHeight) {
        for (uint8_t *dest = pxArrayEnd - pxArrayWidth; dest >= pxArray; dest -= pxArrayWidth, pxData += pxDataWidth) {
            pixel24_t *pxDataArr = (pixel24_t *) pxData;
            for (uint32_t i = 0; i < pxWidth; ++i) {
                dest[i] = pxDataArr[i].r * R_SCALING + pxDataArr[i].g * G_SCALING + pxDataArr[i].b * B_SCALING;
            }
        }
    } else {
        for (uint8_t *dest = pxArray; dest < pxArrayEnd; dest += pxArrayWidth, pxData += pxDataWidth) {
            pixel24_t *pxDataArr = (pixel24_t *) pxData;
            for (uint32_t i = 0; i < pxWidth; ++i) {
                dest[i] = pxDataArr[i].r * R_SCALING + pxDataArr[i].g * G_SCALING + pxDataArr[i].b * B_SCALING;
            }
        }
    }

    _bmpImgBuf->pxArray = pxArray;
    _bmpImgBuf->pxWidth = pxWidth;
    _bmpImgBuf->pxHeight = pxHeight;
    _bmpImgBuf->pxArraySize = pxArraySize;

    return 0;
}


//OUTPUT STUFF

struct __attribute__((__packed__)) writeBMPHeader {
    uint16_t signature;
    uint32_t fileSize;
    uint32_t _0;
    uint32_t dataOffset;
    uint32_t infoHeadersize;
    int32_t pxWidth;
    int32_t pxHeight;
    uint16_t planes;
    uint16_t bitDepth;
    uint32_t _1;
    uint32_t _2;
    int32_t _3;
    int32_t _4;
    uint32_t _5;
    uint32_t _6;
};

struct __attribute__((__packed__)) colorTableEntry {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t _;
};

static void copyPixelData(uint8_t *pxArray, size_t pxArraySize, char *pxData, uint32_t byteWidth, uint32_t byteWidthPadded) {
    uint8_t *pxArrayEnd = pxArray + pxArraySize;

    while (pxArray < pxArrayEnd) {
        memcpy(pxData, pxArray, byteWidth);
        pxArray += byteWidth;
        pxData += byteWidthPadded;
    }
}

/*
Return a pointer to a buffer containing a complete BMP image.
Writes size of buffer to parameter "_size".
*/
char *arrayToBmp(const uBMPImage *bmpImg, size_t *_size) {
    uint32_t byteWidth = bmpImg->pxWidth * sizeof(pixel24_t);
    uint32_t byteWidthPadded = (byteWidth & 0x3) ? ((byteWidth & ~0x3) + 4) : byteWidth;

    *_size = sizeof(struct writeBMPHeader) + byteWidthPadded * (bmpImg->pxHeight);
    char *buf = calloc(*_size, sizeof(uint8_t));
    if (buf == NULL) {
        fprintf(stderr, "Failed allocating memory for buf\n");
        return NULL;
    }

    struct writeBMPHeader *header = (struct writeBMPHeader *) buf;
    header->signature = BMP_HEADER_SIGN;
    header->fileSize = *_size;
    header->dataOffset = sizeof(struct writeBMPHeader);
    header->infoHeadersize = INFO_HEADER_SIZE;
    header->pxWidth = bmpImg->pxWidth;
    header->pxHeight = bmpImg->pxHeight;
    header->planes = 1;
    header->bitDepth = 8 * sizeof(pixel24_t);

    copyPixelData(bmpImg->pxArray, bmpImg->pxArraySize, buf + sizeof(struct writeBMPHeader), byteWidth, byteWidthPadded);
    return buf;
}

/*
Return a pointer to a buffer containing a complete grayscale BMP image.
Writes size of buffer to parameter "_size".
*/
char *arrayToBmp_graysc(const uBMPImage *bmpImg, size_t * _size) {
    uint32_t byteWidth = bmpImg->pxWidth * sizeof(pixel8_t);
    uint32_t byteWidthPadded = (byteWidth & 0x3) ? ((byteWidth & ~0x3) + 4) : byteWidth;

    *_size = sizeof(struct writeBMPHeader) + 256 * sizeof(struct colorTableEntry) + byteWidthPadded * (bmpImg->pxHeight);
    char *buf = calloc(*_size, sizeof(uint8_t));
    if (buf == NULL) {
        fprintf(stderr, "Failed allocating memory for buf\n");
        return NULL;
    }

    struct writeBMPHeader *header = (struct writeBMPHeader *) buf;
    header->signature = BMP_HEADER_SIGN;
    header->fileSize = *_size;
    header->dataOffset = sizeof(struct writeBMPHeader) + 256 * sizeof(struct colorTableEntry);
    header->infoHeadersize = INFO_HEADER_SIZE;
    header->pxWidth = bmpImg->pxWidth;
    header->pxHeight = bmpImg->pxHeight;
    header->planes = 1;
    header->bitDepth = 8 * sizeof(pixel8_t);

    struct colorTableEntry *colorTable = (struct colorTableEntry *) (buf + sizeof(struct writeBMPHeader));
    for (int i = 0; i < 256; ++i) {
        colorTable[i].r = i;
        colorTable[i].g = i;
        colorTable[i].b = i;
    }

    copyPixelData(bmpImg->pxArray, bmpImg->pxArraySize, buf + 256 * sizeof(struct colorTableEntry) + sizeof(struct writeBMPHeader),
                    byteWidth, byteWidthPadded);
    return buf;
}
