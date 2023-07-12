//
// Created by Cato on 03.07.23.
//

#include <stdlib.h>
#include <stdio.h>
#include "test_similarity.h"

#include "../Testsystem/unittest.h"
#include "../Testsystem/image_similarity.h"
#include "../IOSystem/test_functionality.h"

static size_t sobel_buffer_size;
static uBMPImage *sobelImage;
static uint8_t *ref_pixels;
static size_t ref_px_array_size;
static size_t image_height;
static size_t image_width;

static int setup(char *pathSobelImage, size_t height, size_t width, uint8_t *reference_pixel_array, size_t reference_px_array_size) {
    sobelImage = malloc(sizeof(uBMPImage));
    if (sobelImage == NULL) {
        fprintf(stderr, "Failed allocating memory for sobelImage\n");
        return 1;
    }
    sobel_buffer_size = loadPicture(pathSobelImage, sobelImage);
    if (sobel_buffer_size == 0) {
        fprintf(stderr, "Couldn't load sobel image %s\n", pathSobelImage);
        free(sobelImage);
        return 1;
    }
    ref_pixels = reference_pixel_array;
    ref_px_array_size = reference_px_array_size;
    image_height = height;
    image_width = width;
    return 0;
}

static void tearDown() {
    free(sobelImage->pxArray);    
    free(sobelImage);
}

void testSimilarity() {
    double similarity = compareImages(ref_pixels, ref_px_array_size, image_width, image_height, sobelImage->pxArray, sobelImage->pxArraySize);
    ASSERT_EQUAL_DOUBLE(1.0, similarity, 1e-9);
}

int runTestSimilarity(char *pathSobelImage, size_t reference_img_height, size_t reference_img_width, uint8_t *reference_pixel_array, size_t reference_px_array_size) {
    if (setup(pathSobelImage, reference_img_height, reference_img_width, reference_pixel_array, reference_px_array_size) == 1) {
        return 1;
    }
    START_TESTING;
    RUN_TEST(testSimilarity);
    STOP_TESTING;
    tearDown();
    return 0;
}
