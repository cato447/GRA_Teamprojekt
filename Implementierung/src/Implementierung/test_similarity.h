#ifndef _TEST_SIMILARITY_H_
#define _TEST_SIMILARITY_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

int run_test_similarity(uint8_t *sobel_pixel_array, size_t sobel_px_array_size, uint8_t *reference_pixel_array,
                        size_t reference_px_array_size, bool is_graysc);

#endif
