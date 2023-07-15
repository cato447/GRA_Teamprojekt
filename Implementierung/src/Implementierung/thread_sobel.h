#ifndef _THREAD_SOBEL_H_
#define _THREAD_SOBEL_H_

#include <stdint.h>
#include <stddef.h>

void thread_sobel(uint8_t *img_in, size_t width, size_t height, uint8_t *img_out);
void thread_sobel_graysc(uint8_t *img_in, size_t width, size_t height, uint8_t *img_out);

#endif
