#ifndef _BMP_LOADER_H_
#define _BMP_LOADER_H_

#include "s_image.h"

s_image *load_image(const char *path, int in_graysc);
int save_image(s_image *img_buf, int in_graysc, const char *path);

#endif
