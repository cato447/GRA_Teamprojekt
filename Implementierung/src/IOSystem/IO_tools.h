#ifndef IO_TOOLS_H
#define IO_TOOLS_H

#include <stddef.h>
#include "bmp_parser.h"

char* readFile (const char* path, size_t* bufSize);

size_t loadPicture(char *path, uBMPImage *img);

void writeFile (const char* path, char* buf, size_t bufSize);

#endif
