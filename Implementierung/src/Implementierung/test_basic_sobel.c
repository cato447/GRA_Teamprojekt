#include "basic_sobel.h"
#include "../IOSystem/bmp_parser.h"
#include "../Testsystem/unittest.h"
#include "../IOSystem/libbmp.h"

#include "stdio.h"
#include "stdlib.h"

size_t buffer_size;
void* raw_img_data;
uBMPImage *bmpImage;

int testColorOfPixelRed() {
    int x = 1;
    int y = 1;
    return ASSERT_EQUAL_U_INT8(34, colorOfPixel(bmpImage->pxArray, bmpImage->pxWidth, x, y, RED));
}

int testColorOfPixelGreen() {
    int x = 2;
    int y = 2;
    return ASSERT_EQUAL_U_INT8(133, colorOfPixel(bmpImage->pxArray, bmpImage->pxWidth, x, y, GREEN));
}

int testColorOfPixelBlue() {
    int x = 2;
    int y = 1;
    return ASSERT_EQUAL_U_INT8(102, colorOfPixel(bmpImage->pxArray, bmpImage->pxWidth, x,y,BLUE));
}

int testSetColorOfPixel() {
    uint8_t red = 32;
    uint8_t green = 42;
    uint8_t blue = 52;
    int x = 6;
    int y = 7;
    setPixelAt(bmpImage->pxArray, bmpImage->pxWidth, 6, 7, red, green, blue);
    int passed = 1;
    passed &= ASSERT_EQUAL_U_INT8(red, colorOfPixel(bmpImage->pxArray, bmpImage->pxWidth, x, y, RED));
    passed &= ASSERT_EQUAL_U_INT8(green, colorOfPixel(bmpImage->pxArray, bmpImage->pxWidth, x, y, GREEN));
    passed &= ASSERT_EQUAL_U_INT8(blue, colorOfPixel(bmpImage->pxArray, bmpImage->pxWidth, x,y,BLUE));
    return passed;
}

int testSobelSmall() {
    pixel24_t* newPixels = malloc(sizeof(bmpImage->pxArray));
    sobel(bmpImage->pxArray, bmpImage->pxWidth, bmpImage->pxHeight, newPixels);
    bmp_img img;
    bmp_img_init_df(&img, bmpImage->pxWidth, bmpImage->pxHeight);
    for (size_t y = 0; y < bmpImage->pxWidth; y++)
    {
        for (size_t x = 0; x < bmpImage->pxHeight; x++)
        {
            bmp_pixel_init (&img.img_pixels[x][y], newPixels->r, newPixels->g, newPixels->b);
            newPixels++;
        }
    }
    bmp_img_write (&img, "test.bmp");
    bmp_img_free (&img);
    return 0;
}



int setUp() {
    char* path = "../res/johnmuirtrail.bmp";
    fprintf(stdout, "Loading BMP File: %s for test data\n", path);
    raw_img_data = readBMPFile(path, &buffer_size);

    if (raw_img_data == NULL){
        fprintf(stderr, "Couldn't read BMP File\n");
        return 1;
    }
    bmpImage = malloc(sizeof(uBMPImage));
    if (bmpImage == NULL){
        fprintf(stderr, "Couldn't allocate memory for BMP Image\n");
        return 1;
    }
    if (parseBMPFile(raw_img_data, buffer_size, bmpImage) == 1) {
        fprintf(stderr, "Couldn't parse BMP file");
        return 1;
    }
    return 0;
}

int tearDown() {
    free(raw_img_data);
    free(bmpImage);
    return 0;
}

int runTests(void){
    startTesting(__BASE_FILE__);
    if (setUp() == 1){
        tearDown();
        return 1;
    }
    runTest(testColorOfPixelRed);
    runTest(testColorOfPixelBlue);
    runTest(testColorOfPixelGreen);
    runTest(testSetColorOfPixel);
    testSobelSmall();
    stopTesting();
    tearDown();
    return 0;
}

int main(void){
    runTests();

}


