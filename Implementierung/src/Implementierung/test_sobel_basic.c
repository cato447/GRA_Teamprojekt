#include "test_sobel_basic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../Implementierung/sobel_basic.h"
#include "../Testsystem/unittest.h"
#include "../Testsystem/image_similarity.h"
#include "../IOSystem/bmp_loader.h"

#define MUSTER_PATH "../res/raw/muster.bmp"
#define MUSTER_REF_PATH "../res/reference/muster_sobel.bmp"

// Data tests are run on
static size_t buffer_size;
static s_image *bmp_image;

static int free_bmp_image(s_image *img) {
    free(img->px_array);
    free(img);
    return 0;
}

void test_color_of_pixel_red(void) {
    int x = 1;
    int y = 1;
    ASSERT_EQUAL_UINT8(34, color_of_pixel(bmp_image->px_array, bmp_image->px_width, x, y, RED));
}

void test_color_of_pixel_green(void) {
    int x = 2;
    int y = 2;
    ASSERT_EQUAL_UINT8(133, color_of_pixel(bmp_image->px_array, bmp_image->px_width, x, y, GREEN));
}

void test_color_of_pixel_blue(void) {
    int x = 2;
    int y = 1;
    ASSERT_EQUAL_UINT8(102, color_of_pixel(bmp_image->px_array, bmp_image->px_width, x, y, BLUE));
}

void test_set_color_of_pixel(void) {
    uint8_t red = 32;
    uint8_t green = 42;
    uint8_t blue = 52;
    int x = 0;
    int y = 0;

    uint8_t *px_array_copy = malloc(bmp_image->px_array_size);
    if (px_array_copy == NULL) {
        FAIL("Failed allocating memory for px_array_copy\n");
        return;
    }
    memcpy(px_array_copy, bmp_image->px_array, bmp_image->px_array_size);

    set_pixel_at(px_array_copy, bmp_image->px_width, x, y, red, green, blue);
    ASSERT_EQUAL_UINT8(red, color_of_pixel(px_array_copy, bmp_image->px_width, x, y, RED));
    ASSERT_EQUAL_UINT8(green, color_of_pixel(px_array_copy, bmp_image->px_width, x, y, GREEN));
    ASSERT_EQUAL_UINT8(blue, color_of_pixel(px_array_copy, bmp_image->px_width, x, y, BLUE));

    free(px_array_copy);
}

void test_sobel(void) {
    uint8_t *new_pixels = calloc(bmp_image->px_array_size, sizeof(uint8_t));
    if (new_pixels == NULL) {
        FAIL("Failed allocating memory for new_pixels\n");
        return;
    }
    sobel((uint8_t *) bmp_image->px_array, bmp_image->px_width, bmp_image->px_height, (uint8_t *) new_pixels);

    s_image *reference_bmp_image = malloc(sizeof(s_image));
    if (reference_bmp_image == NULL) {
        FAIL("Failed allocating memory for reference_bmp_image\n");
        free(new_pixels);
        return;
    }
    
    size_t reference_buffer_size = load_picture(MUSTER_REF_PATH, reference_bmp_image);
    if (reference_buffer_size == 0) {
        FAIL("Failed loading reference picture\n");
        free(new_pixels);
        free(reference_bmp_image);
        return;
    }

    double similarity = compare_images(new_pixels, bmp_image->px_array_size,
                                      reference_bmp_image->px_array, reference_bmp_image->px_array_size);

    free_bmp_image(reference_bmp_image);
    free(new_pixels);

    ASSERT_EQUAL_DOUBLE(1.0, similarity, 1e-9);
}

static int setup(void) {
    bmp_image = malloc(sizeof(s_image));
    if (bmp_image == NULL) {
        fprintf(stderr, "Failed allocating memory for bmp_image\n");
        return 1;
    }
    buffer_size = load_picture(MUSTER_PATH, bmp_image);
    if (buffer_size == 0) {
        free(bmp_image);
        return 1;
    }
    return 0;
}

static void tear_down() {
    free_bmp_image(bmp_image);
}

int run_tests_sobel(void) {
    if (setup() == 1) {
        return 1;
    }

    START_TESTING();
    RUN_TEST(test_color_of_pixel_red);
    RUN_TEST(test_color_of_pixel_blue);
    RUN_TEST(test_color_of_pixel_green);
    RUN_TEST(test_set_color_of_pixel);
    RUN_TEST(test_sobel);
    STOP_TESTING_VERBOSE();

    tear_down();
    return 0;
}
