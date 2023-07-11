#include "test_basic_sobel.h"

#include "../Implementierung/basic_sobel.h"
#include "../Testsystem/unittest.h"
#include "../Testsystem/image_similarity.h"
#include "../IOSystem/test_functionality.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MUSTER_PATH "../res/raw/muster.bmp"
#define MUSTER_REF_PATH "../res/reference/muster_sobel.bmp"

// Data tests are run on
static size_t buffer_size;
static uBMPImage *bmpImage;

int freeBmpImg(uBMPImage *img) {
    free(img->pxArray);
    free(img);
    return 0;
}

int testColorOfPixelRed() {
    int x = 1;
    int y = 1;
    return ASSERT_EQUAL_U_INT8(34, colorOfPixel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, x, y, RED));
}

int testColorOfPixelGreen() {
    int x = 2;
    int y = 2;
    return ASSERT_EQUAL_U_INT8(133, colorOfPixel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, x, y, GREEN));
}

int testColorOfPixelBlue() {
    int x = 2;
    int y = 1;
    return ASSERT_EQUAL_U_INT8(102, colorOfPixel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, x, y, BLUE));
}

int testSetColorOfPixel() {
    uint8_t red = 32;
    uint8_t green = 42;
    uint8_t blue = 52;
    int x = 0;
    int y = 0;
    setPixelAt((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, x, y, red, green, blue);
    int passed = 1;
    passed &= ASSERT_EQUAL_U_INT8(red, colorOfPixel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, x, y, RED));
    passed &= ASSERT_EQUAL_U_INT8(green, colorOfPixel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, x, y, GREEN));
    passed &= ASSERT_EQUAL_U_INT8(blue, colorOfPixel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, x, y, BLUE));
    return passed;
}

int testSobel() {
    uint8_t *newPixels = calloc(bmpImage->pxArraySize, sizeof(uint8_t));
    if (newPixels == NULL) {
        fprintf(stderr, "Failed allocating memory for newPixels\n");
        return 1;
    }
    sobel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight, (uint8_t *) newPixels);

    uBMPImage *reference_bmpImage = malloc(sizeof(uBMPImage));
    if (reference_bmpImage == NULL) {
        fprintf(stderr, "Failed allocating memory for reference_bmpImage\n");
        free(newPixels);
        return 1;
    }
    size_t reference_buffer_size = loadPicture(MUSTER_REF_PATH, reference_bmpImage);

    if (reference_buffer_size == 0) {
        fprintf(stderr, "Couldn't load picture\n");
        free(newPixels);
        freeBmpImg(reference_bmpImage);
        return 1;
    }

    double similarity = compareImages(newPixels, bmpImage->pxArraySize, bmpImage->pxWidth, bmpImage->pxHeight,
                                      reference_bmpImage->pxArray, reference_bmpImage->pxArraySize);

    freeBmpImg(reference_bmpImage);
    free(newPixels);

    return ASSERT_EQUAL_DOUBLE(1.0, similarity, 1e-9);
}

int setUp() {
    bmpImage = malloc(sizeof(uBMPImage));
    if (bmpImage == NULL) {
        fprintf(stderr, "Failed allocating memory for bmpImage\n");
        return 1;
    }
    buffer_size = loadPicture(MUSTER_PATH, bmpImage);
    if (buffer_size == 0) {
        free(bmpImage);
        return 1;
    }
    return 0;
}

void tearDown() {
    freeBmpImg(bmpImage);
}

int runTestsSobel(void) {
    if (setUp() == 1) {
        return 1;
    }
    startTesting(__BASE_FILE__);
    runTest(testColorOfPixelRed);
    runTest(testColorOfPixelBlue);
    runTest(testColorOfPixelGreen);
    runTest(testSetColorOfPixel);
    testSobel();
    tearDown();
    stopTesting();
    return 0;
}
