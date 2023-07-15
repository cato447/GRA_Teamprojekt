#ifndef _BMP_LOADER_H_
#define _BMP_LOADER_H_

#include <stddef.h>

#include "bmp_definitions.h"

size_t load_picture(const char *path, s_image *_img);
size_t load_picture_graysc(const char *path, s_image *_img);

#endif
