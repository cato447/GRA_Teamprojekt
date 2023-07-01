#ifndef BMP_PARSER_H
#define BMP_PARSER_H

#include "bmp_definitions.h"
#include <stddef.h>

int bmpToArray (char* buf, size_t bufSize, uBMPImage* bmpImgBuf);
char* arrayToBmp (const uBMPImage* bmpImgBuf, size_t* bufSize);

#endif
