#include "bmpDefinitions.h"

int bmpToArray (const void* buf, size_t bufSize, uBMPImage* bmpImgBuf);
void* arrayToBmp (const uBMPImage* bmpImgBuf, size_t* bufSize);
