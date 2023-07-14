//
// Created by Cato on 03.07.23.
//

#ifndef IMPLEMENTIERUNG_TEST_SIMILARITY_H
#define IMPLEMENTIERUNG_TEST_SIMILARITY_H
#include "../IOSystem/bmp_definitions.h"
#include <stdbool.h>
int runTestSimilarity(uint8_t *sobel_pixel_array, size_t sobel_px_array_size, uint8_t *reference_pixel_array, size_t reference_px_array_size, bool isGraysc);
#endif //IMPLEMENTIERUNG_TEST_SIMILARITY_H
