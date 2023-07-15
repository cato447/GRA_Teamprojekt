#ifndef _TEST_FUNCTIONALITY_H_
#define _TEST_FUNCTIONALITY_H_

#include <stddef.h>

#include "bmp_definitions.h"

size_t load_picture(char *path, s_image *img);
size_t load_picture_graysc(char *path, s_image *img);

#endif
