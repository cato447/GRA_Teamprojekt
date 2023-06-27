#include "../Implementierung/basic_sobel.h"
#include "../Testsystem/unittest.h"
#include "../Testsystem/image_similarity.h"

#include "stdio.h"
#include "stdlib.h"

// Data tests are run on
static size_t img_size;
static uBMPImage *bmpImage;

size_t loadPicture(char *path, uBMPImage *img) {
    void *buffer;
    size_t buffer_size;
    fprintf(stdout, "Loading BMP File: %s for test data\n", path);
    buffer = readBMPFile(path, &buffer_size);

    if (buffer == NULL) {
        fprintf(stderr, "Couldn't read BMP File\n");
        return 0;
    }
    if (img == NULL) {
        fprintf(stderr, "img was not initialized\n");
        return 0;
    }
    if (parseBMPFile(buffer, buffer_size, img) == 1) {
        fprintf(stderr, "Couldn't parse BMP file");
        free(buffer);
        return 0;
    }
    free(buffer);
    return buffer_size;
}

int freeBmpImg(uBMPImage *img){
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
    setPixelAt((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, 6, 7, red, green, blue);
    int passed = 1;
    passed &= ASSERT_EQUAL_U_INT8(red, colorOfPixel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, x, y, RED));
    passed &= ASSERT_EQUAL_U_INT8(green, colorOfPixel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, x, y, GREEN));
    passed &= ASSERT_EQUAL_U_INT8(blue, colorOfPixel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, x, y, BLUE));
    return passed;
}

int testSobel() {
    //TODO: Get real testing data
    pixel24_t *newPixels = malloc(bmpImage->pxHeight * bmpImage->pxWidth * sizeof(pixel24_t));
    sobel((uint8_t *) bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight, (uint8_t *) newPixels);

    char *reference_path = "../res/muster.bmp";

    size_t reference_buffer_size;
    uBMPImage *reference_bmpImage = malloc(sizeof(uBMPImage));
    if (reference_bmpImage == NULL) {
        fprintf(stderr, "Couldn't allocated reference_bmpImage\n");
        free(newPixels);
        return 1;
    }

    reference_buffer_size = loadPicture(reference_path, reference_bmpImage);

    if (reference_buffer_size == 0) {
        fprintf(stderr, "Couldn't load picture\n");
        free(newPixels);
        freeBmpImg(reference_bmpImage);
        return 1;
    }

    double similarity = compareImages((uint8_t *) bmpImage->pxArray,
                                      bmpImage->pxWidth * bmpImage->pxHeight * 3,
                                      (uint8_t *) reference_bmpImage->pxArray,
                                      reference_bmpImage->pxWidth * reference_bmpImage->pxHeight * 3);
    printf("similarity: %3.3f %%\n", (similarity * 100));
    freeBmpImg(reference_bmpImage);
    free(newPixels);
    return 0;
}

int setUp() {
    char *path = "../res/muster.bmp";
    bmpImage = malloc(sizeof(uBMPImage));
    img_size = loadPicture(path, bmpImage);
    if (img_size == 0) {
        return 1;
    }
    return 0;
}

int tearDown() {
    freeBmpImg(bmpImage);
    return 0;
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
    stopTesting();
    tearDown();
    return 0;
}
