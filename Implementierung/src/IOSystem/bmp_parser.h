#ifndef _BMP_PARSER_H_
#define _BMP_PARSER_H_

#include <stddef.h>

#include "s_image.h"

int bmp_to_array(const char *buf, size_t buf_size, s_image *_bmp_img_buf);
int bmp_to_array_graysc(const char *buf, size_t buf_size, s_image *_bmp_img_buf);

char *array_to_bmp(const s_image *bmp_img_buf, size_t *_buf_size);
char *array_to_bmp_graysc(const s_image *bmp_img_buf, size_t *_buf_size);

#endif
