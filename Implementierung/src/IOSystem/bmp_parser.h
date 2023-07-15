#ifndef _BMP_PARSER_H_
#define _BMP_PARSER_H_

#include <stddef.h>

#include "bmp_definitions.h"

int bmp_to_array (char *buf, size_t buf_size, s_image *_bmp_img_buf);
int bmp_to_array_graysc (char *buf, size_t buf_size, s_image *_bmp_img_buf);

char *array_to_bmp (const s_image *bmp_img_buf, size_t *_buf_size);
char *array_to_bmp_graysc (const s_image *bmp_img_buf, size_t *_buf_size);

#endif
