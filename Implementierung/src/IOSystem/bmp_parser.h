#ifndef BMP_PARSER_H
#define BMP_PARSER_H

#include "bmp_definitions.h"
#include <stddef.h>

int bmpToArray (char *buf, size_t bufSize, uBMPImage *_bmpImgBuf);
int bmpToArray_graysc (char *buf, size_t bufSize, uBMPImage *_bmpImgBuf);

char *arrayToBmp (const uBMPImage *bmpImgBuf, size_t *_bufSize);
char *arrayToBmp_graysc (const uBMPImage *bmpImgBuf, size_t *_bufSize);

#endif
