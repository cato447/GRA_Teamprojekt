#include "test_similarity.h"

#include <stdlib.h>
#include <stdio.h>

#include "../Testsystem/unittest.h"
#include "../Testsystem/image_similarity.h"
#include "../IOSystem/test_functionality.h"

static uint8_t *sobel_pixels;
static size_t sobel_buffer_size;
static uint8_t *ref_pixels;
static size_t ref_px_array_size;

static void test_similarity() {
    double similarity = compare_images(ref_pixels, ref_px_array_size,sobel_pixels, sobel_buffer_size);
    ASSERT_EQUAL_DOUBLE(1.0, similarity, 1e-9);
}

static void test_similarity_graysc() {
    double similarity = compare_images_graysc(ref_pixels, ref_px_array_size,sobel_pixels, sobel_buffer_size);
    ASSERT_EQUAL_DOUBLE(1.0, similarity, 1e-9);
}

int run_test_similarity(uint8_t *sobel_pixel_array, size_t sobel_px_array_size, uint8_t *reference_pixel_array, size_t reference_px_array_size, bool is_graysc) {
    sobel_pixels = sobel_pixel_array;
    sobel_buffer_size = sobel_px_array_size;
    ref_pixels = reference_pixel_array;
    ref_px_array_size = reference_px_array_size;
    START_TESTING();
    if (is_graysc) {
        RUN_TEST(test_similarity_graysc);
    } else {
        RUN_TEST(test_similarity);
    }
    STOP_TESTING();
    return 0;
}
