#ifndef _IMAGE_SIMILARITY_H_
#define _IMAGE_SIMILARITY_H_

#include <stdint.h>
#include <stddef.h>

double compare_images(const uint8_t *img_1, size_t img_1_size, const uint8_t *img_2, size_t img_2_size);
double compare_images_graysc(const uint8_t *img_1, size_t img1_size, const uint8_t *img_2, size_t img_2_size);

#endif
