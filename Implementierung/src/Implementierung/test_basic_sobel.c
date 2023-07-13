#include "test_basic_sobel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../Implementierung/basic_sobel.h"
#include "../Testsystem/unittest.h"
#include "../Testsystem/image_similarity.h"
#include "../IOSystem/test_functionality.h"

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

void testColorOfPixelRed() {
    int x = 1;
    int y = 1;
    ASSERT_EQUAL_U_INT8(34, colorOfPixel(bmpImage->pxArray, bmpImage->pxWidth, x, y, RED));
}

void testColorOfPixelGreen() {
    int x = 2;
    int y = 2;
    ASSERT_EQUAL_U_INT8(133, colorOfPixel(bmpImage->pxArray, bmpImage->pxWidth, x, y, GREEN));
}

void testColorOfPixelBlue() {
    int x = 2;
    int y = 1;
    ASSERT_EQUAL_U_INT8(102, colorOfPixel(bmpImage->pxArray, bmpImage->pxWidth, x, y, BLUE));
}

void testSetColorOfPixel() {
    uint8_t red = 32;
    uint8_t green = 42;
    uint8_t blue = 52;
    int x = 0;
    int y = 0;

    uint8_t* pxArrayCopy = malloc(bmpImage->pxArraySize);
    if (pxArrayCopy == NULL) {
        FAIL("Failed allocating memory for pxArrayCopy\n");
        return;
    }

    setPixelAt(pxArrayCopy, bmpImage->pxWidth, x, y, red, green, blue);
    ASSERT_EQUAL_U_INT8(red, colorOfPixel(pxArrayCopy, bmpImage->pxWidth, x, y, RED));
    ASSERT_EQUAL_U_INT8(green, colorOfPixel(pxArrayCopy, bmpImage->pxWidth, x, y, GREEN));
    ASSERT_EQUAL_U_INT8(blue, colorOfPixel(pxArrayCopy, bmpImage->pxWidth, x, y, BLUE));

    free(pxArrayCopy);
}

void testSobel() {
    uint8_t *newPixels = calloc(bmpImage->pxArraySize, sizeof(uint8_t));
    if (newPixels == NULL) {
        FAIL("Failed allocating memory for newPixels\n");
        return;
    }
    sobel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight, (uint8_t *) newPixels);

    uBMPImage *reference_bmpImage = malloc(sizeof(uBMPImage));
    if (reference_bmpImage == NULL) {
        FAIL("Failed allocating memory for reference_bmpImage\n");
        free(newPixels);
        return;
    }
    
    size_t reference_buffer_size = loadPicture(MUSTER_REF_PATH, reference_bmpImage);
    if (reference_buffer_size == 0) {
        FAIL("Failed loading reference picture\n");
        free(newPixels);
        free(reference_bmpImage);
        return;
    }

    double similarity = compareImages(newPixels, bmpImage->pxArraySize,
                                      reference_bmpImage->pxArray, reference_bmpImage->pxArraySize);

    freeBmpImg(reference_bmpImage);
    free(newPixels);

    ASSERT_EQUAL_DOUBLE(1.0, similarity, 1e-9);
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
    START_TESTING;
    RUN_TEST(testColorOfPixelRed);
    RUN_TEST(testColorOfPixelBlue);
    RUN_TEST(testColorOfPixelGreen);
    RUN_TEST(testSetColorOfPixel);
    RUN_TEST(testSobel);
    STOP_TESTING;
    tearDown();
    return 0;
}
