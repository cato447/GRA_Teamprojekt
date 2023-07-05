#include "test_basic_sobel.h"

#include "../Implementierung/basic_sobel.h"
#include "../Testsystem/unittest.h"
#include "../Testsystem/image_similarity.h"
#include "../IOSystem/test_functionality.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Declares and initializes OUT as a char*.
Sets OUT to [directory containing this file] ^ CONCAT.
*/
#define CONCAT_TO_DIR_OF_FILE(CONCAT, OUT) \
    char ____dir[sizeof(__FILE__)]; \
    strcpy(____dir, __FILE__); \
    char* ____c = strrchr(____dir, '/'); \
    *____c = '\0'; \
    char OUT[strlen(____dir) + strlen(CONCAT) + 1]; \
    strcpy(OUT, ____dir); \
    strcat(OUT, CONCAT); \

#define MUSTER_PATH_REL(OUT) CONCAT_TO_DIR_OF_FILE("/../../res/raw/muster.bmp", OUT)
#define MUSTER_REF_PATH_REL(OUT) CONCAT_TO_DIR_OF_FILE("/../../res/reference/muster_sobel.bmp", OUT)

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
    int x = 6;
    int y = 7;
    setPixelAt((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, x, y, red, green, blue);
    int passed = 1;
    passed &= ASSERT_EQUAL_U_INT8(red, colorOfPixel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, x, y, RED));
    passed &= ASSERT_EQUAL_U_INT8(green, colorOfPixel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, x, y, GREEN));
    passed &= ASSERT_EQUAL_U_INT8(blue, colorOfPixel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, x, y, BLUE));
    return passed;
}

int testSobel() {
    pixel24_t *newPixels = malloc(bmpImage->pxArraySize * sizeof(pixel24_t));
    sobel((uint8_t*) bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight, (uint8_t *) newPixels);

    MUSTER_REF_PATH_REL(reference_path);

    uBMPImage *reference_bmpImage = malloc(sizeof(uBMPImage));
    if (reference_bmpImage == NULL) {
        fprintf(stderr, "Couldn't allocated reference_bmpImage\n");
        free(newPixels);
        return 1;
    }
    size_t reference_buffer_size = loadPicture(reference_path, reference_bmpImage);

    if (reference_buffer_size == 0) {
        fprintf(stderr, "Couldn't load picture\n");
        free(newPixels);
        freeBmpImg(reference_bmpImage);
        return 1;
    }

    double similarity = compareImages((uint8_t *) newPixels,
                                      bmpImage->pxArraySize * sizeof(pixel24_t),
                                      (uint8_t *) reference_bmpImage->pxArray,
                                      reference_bmpImage->pxArraySize * sizeof(pixel24_t));

    freeBmpImg(reference_bmpImage);
    free(newPixels);

    return ASSERT_EQUAL_DOUBLE(1.0, similarity, 1e-9);
}

int setUp() {
    MUSTER_PATH_REL(path);
    bmpImage = malloc(sizeof(uBMPImage));
    buffer_size = loadPicture(path, bmpImage);
    if (buffer_size == 0) {
        return 1;
    }
    return 0;
}

void tearDown() {
    freeBmpImg(bmpImage);
}

int runTestsSobel(void) {
    startTesting(__BASE_FILE__);
    if (setUp() == 1) {
        tearDown();
        return 1;
    }
    runTest(testColorOfPixelRed);
    runTest(testColorOfPixelBlue);
    runTest(testColorOfPixelGreen);
    runTest(testSetColorOfPixel);
    testSobel();
    tearDown();
    stopTesting();
    return 0;
}
