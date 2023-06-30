#include "bmp_parser.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

/*
Returns a pointer to the data buffer of the file read at parameter "path" and sets "bufSize" accordingly on successful read.
*/
void* readBMPFile(const char* path, size_t* bufSize) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Error trying to open file at \"%s\": %s\n", path, strerror(errno));
        return NULL;
    }

    struct stat statbuf;
    if (fstat(fileno(file), &statbuf)) {
        fprintf(stderr, "Error trying to check stats of file at %s: %s\n", path, strerror(errno));
        fclose(file);
        return NULL;
    }
    if (!S_ISREG(statbuf.st_mode)) {
        fprintf(stderr, "Error: file at %s isn't a regular file\n", path);
        fclose(file);
        return NULL;
    }

    void* buf = malloc(statbuf.st_size);
    if (!buf) {
        fprintf(stderr, "Error: failed allocating memory for file buffer\n");
        fclose(file);
        return NULL;
    }

    if (fread(buf, 1, statbuf.st_size, file) != statbuf.st_size) {
        fprintf(stderr, "Error: failed reading file data at %s\n", path);
        fclose(file);
        free(buf);
        return NULL;
    }

    *bufSize = statbuf.st_size;
    return buf;
}


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
int parseBMPFile(const void* buf, size_t bufSize, uBMPImage* bmpImgBuf) {
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

    pixel24_t* pxArray = calloc(byteWidth * pxHeight, sizeof(uint8_t));
    if (!pxArray) {
        fprintf(stderr, "Error: failed allocating memory for pixel array\n");
        return 1;
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
