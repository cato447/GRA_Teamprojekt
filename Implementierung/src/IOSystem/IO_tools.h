#ifndef IO_TOOLS_H
#define IO_TOOLS_H

#include <stddef.h>
#include "bmp_parser.h"

char* readFile (const char* path, size_t* bufSize);
int writeFile (const char* path, char* buf, size_t bufSize);

#endif
