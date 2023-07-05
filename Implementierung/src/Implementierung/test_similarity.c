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

static void freeBmpImg(uBMPImage *img){
    free(img->pxArray);
    free(img);
}

static int setup(char *pathSobelImage, uint8_t *reference_pixel_array, size_t reference_px_array_size) {
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
    return 0;
}

static void tearDown() {
    freeBmpImg(sobelImage);
}

static int testSimilarity() {
    double similarity = compareImages(ref_pixels, ref_px_array_size,
                                      (uint8_t *) sobelImage->pxArray, sobelImage->pxArraySize * sizeof(pixel24_t));
    return ASSERT_EQUAL_DOUBLE(1.0, similarity, 1e-9);
}

int runTestSimilarity(char *pathSobelImage, uint8_t *reference_pixel_array, size_t reference_px_array_size) {
    if (setup(pathSobelImage, reference_pixel_array, reference_px_array_size) == 1) {
        return 1;
    }
    startTesting(__BASE_FILE__);
    runTest(testSimilarity);
    tearDown();
    stopTesting();
    return 0;
}
