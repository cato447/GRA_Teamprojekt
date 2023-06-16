#include "bmp_parser.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#define BMP_FILE_ERROR_VAL &(struct BMPFile) {.size = 0, .buf = NULL}
#define BMP_PARSE_ERROR_VAL &(uBMPImage) {.width = 0, .height = 0, .pxArray = NULL}

/*
Returns the size and a pointer to the data buffer of the file read at parameter "path"
*/
struct BMPFile* readBMPFile(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Error trying to open file at %s!\n", path);
        return BMP_FILE_ERROR_VAL;
    }

    struct stat statbuf;

    if (fstat(fileno(file), &statbuf)) {
        fprintf(stderr, "Error trying to check stats of file at %s!\n", path);
        fclose(file);
        return BMP_FILE_ERROR_VAL;
    }
    if (!S_ISREG(statbuf.st_mode)) {
        fprintf(stderr, "File at %s isn't a regular file!\n", path);
        fclose(file);
        return BMP_FILE_ERROR_VAL;
    }

    void* buf = malloc(statbuf.st_size);
    if (!buf) {
        fprintf(stderr, "Failed allocating memory for file buffer!\n");
        fclose(file);
        return BMP_FILE_ERROR_VAL;
    }

    if (fread(buf, 1, statbuf.st_size, file) != statbuf.st_size) {
        fprintf(stderr, "Error trying to read file at %s!\n", path);
        fclose(file);
        free(buf);
        return BMP_FILE_ERROR_VAL;
    }
    return &(struct BMPFile) {.size = statbuf.st_size, .buf = buf};
}

/*
Returns width, height and an unpadded copy of the pixelarray of the parameter "bmpFile"
*/
uBMPImage* parseBMPFile(struct BMPFile* bmpFile) {
    void* buf = bmpFile->buf;
    size_t size = bmpFile->size;

    if (size < 26 || 
        *(uint32_t*)(buf + 0x02) != size) {
        fprintf(stderr, "File too small / size not matching file info!\n");
        return BMP_PARSE_ERROR_VAL;
    }

    uint32_t dataOffset = *(uint32_t*)(buf + 0x0a);
    int32_t pxWidth = *(int32_t*)(buf + 0x12);
    int32_t pxHeight = *(int32_t*)(buf + 0x16);

    int32_t byteWidth = pxWidth * sizeof(pixel24_t);
    int32_t byteWidthPadded = (byteWidth & 0x3) ? ((byteWidth & ~0x3) + 4) : byteWidth;

    if (dataOffset + byteWidthPadded * pxHeight > size) {
        fprintf(stderr, "File size doesn't match file info!\n");
        return BMP_PARSE_ERROR_VAL;
    }
    
    pixel24_t* pxArray = malloc(byteWidth * pxHeight);
    if (!pxArray) {
        fprintf(stderr, "Failed allocating memory for pixel array");
        return BMP_PARSE_ERROR_VAL;
    }
    
    pixel24_t* pxArrayEnd = pxArray + pxWidth * pxHeight;
    buf += dataOffset;
    for (pixel24_t* dest = pxArray; dest < pxArrayEnd; dest += pxWidth, buf += byteWidthPadded) {
        memcpy(dest, buf, byteWidth);
    }

    return &(uBMPImage) {.width = pxWidth, .height = pxHeight, .pxArray = pxArray};
}
