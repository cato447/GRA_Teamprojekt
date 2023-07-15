#ifndef _THREAD_SOBEL_H_
#define _THREAD_SOBEL_H_

#include <stdint.h>
#include <stddef.h>

void sobel_thread(const uint8_t *img_in, size_t width, size_t height, uint8_t *img_out);
void sobel_thread_graysc(const uint8_t *img_in, size_t width, size_t height, uint8_t *img_out);

#endif
