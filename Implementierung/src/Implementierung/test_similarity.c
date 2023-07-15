//
// Created by Cato on 03.07.23.
//

#include <stdlib.h>
#include <stdio.h>
#include "test_similarity.h"

#include "../Testsystem/unittest.h"
#include "../Testsystem/image_similarity.h"
#include "../IOSystem/test_functionality.h"
static uint8_t *sobel_pixels;
static size_t sobel_buffer_size;
static uint8_t *ref_pixels;
static size_t ref_px_array_size;

static void testSimilarity() {
    double similarity = compareImages(ref_pixels, ref_px_array_size,sobel_pixels, sobel_buffer_size);
    ASSERT_EQUAL_DOUBLE(1.0, similarity, 1e-9);
}

static void testSimilarity_graysc() {
    double similarity = compareImages_graysc(ref_pixels, ref_px_array_size,sobel_pixels, sobel_buffer_size);
    ASSERT_EQUAL_DOUBLE(1.0, similarity, 1e-9);
}

int runTestSimilarity(uint8_t *sobel_pixel_array, size_t sobel_px_array_size, uint8_t *reference_pixel_array, size_t reference_px_array_size, bool isGraysc) {
    sobel_pixels = sobel_pixel_array;
    sobel_buffer_size = sobel_px_array_size;
    ref_pixels = reference_pixel_array;
    ref_px_array_size = reference_px_array_size;
    START_TESTING();
    if (isGraysc) {
        RUN_TEST(testSimilarity_graysc);
    } else {
        RUN_TEST(testSimilarity);
    }
    STOP_TESTING();
    return 0;
}
