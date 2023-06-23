#include "../bmpDefinitions.h"
#include <stddef.h>

int bmpToArray (void* buf, size_t bufSize, uBMPImage* bmpImgBuf);
void* arrayToBmp (const uBMPImage* bmpImgBuf, size_t* bufSize);
