#include "test_IO_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Testsystem/unittest.h"
#include "IO_tools.h"
#include "bmp_parser.h"

void parseFailure(const char *path) {
    size_t bufSize;
    char *buf = readFile(path, &bufSize);
    if (buf == NULL) {
        FAIL("Failed to load test file");
        return;
    }
    uBMPImage bmpImg;
    if (bmpToArray(buf, bufSize, &bmpImg)) {
        PASS();
    } else {
        FAIL("Managed to parse invalid file");
        free(bmpImg.pxArray);
    }

    free(buf);
}

void testFileError() {
    size_t bufSize;
    char *buf = readFile("/mostLikelyDoesntExist", &bufSize);
    if (buf != NULL) {
        FAIL("Managed to read non-existing file");
        free(buf);
    } else {
        PASS();
    }
    buf = readFile("/", &bufSize);
    if (buf != NULL) {
        FAIL("Managed to read invalid file");
        free(buf);
    } else {
        PASS();
    }
}

void testInvalidHeaderSignature() {
    parseFailure("res/parserTests/invalidHeaderSignature.bmp");
}

void testTooSmallForHeader() {
    parseFailure("res/parserTests/tooSmallForHeader.bmp");
}

void testPicTooSmall() {
    parseFailure("res/parserTests/picTooSmall.bmp");
}

void testWidthHeight() {
    size_t bufSize;
    char *buf = readFile("res/parserTests/testWidthHeight.bmp", &bufSize);
    if (buf == NULL) {
        FAIL("Failed to load test file");
        return;
    }

    uBMPImage bmpImg;
    if (bmpToArray(buf, bufSize, &bmpImg)) {
        FAIL("Failed to parse test file");
        free(buf);
        return;
    }
    free(buf);

    ASSERT_EQUAL_U_INT32(199, bmpImg.pxWidth);
    ASSERT_EQUAL_U_INT32(323, bmpImg.pxHeight);
    ASSERT_EQUAL_SIZE_T(199  *323  *3, bmpImg.pxArraySize);

    free(bmpImg.pxArray);
}

void testNegativeWidthHeight() {
    size_t bufSizeTest;
    char *bufTest = readFile("res/parserTests/negativeHeight.bmp", &bufSizeTest);
    if (bufTest == NULL) {
        FAIL("Failed to load test file");
        return;
    }

    size_t bufSizeRef;
    char *bufRef = readFile("res/parserTests/negativeHeightRef.bmp", &bufSizeRef);
    if (bufRef == NULL) {
        free(bufTest);
        FAIL("Failed to load reference file");
        return;
    }

    uBMPImage bmpImgTest;
    if (bmpToArray(bufTest, bufSizeTest, &bmpImgTest)) {
        FAIL("Failed to parse test file");
        free(bufTest);
        free(bufRef);
        return;
    }
    free(bufTest);

    

    uBMPImage bmpImgRef;
    if (bmpToArray(bufRef, bufSizeRef, &bmpImgRef)) {
        FAIL("Failed to parse reference file");
        free(bmpImgTest.pxArray);
        free(bufRef);
        return;
    }
    free(bufRef);

    ASSERT_EQUAL_U_INT32(bmpImgRef.pxWidth, bmpImgTest.pxWidth);
    ASSERT_EQUAL_U_INT32(bmpImgRef.pxHeight, bmpImgTest.pxHeight);

    if (ASSERT_EQUAL_SIZE_T(bmpImgRef.pxArraySize, bmpImgTest.pxArraySize)) {
        free(bmpImgRef.pxArray);
        free(bmpImgTest.pxArray);
        return;
    }

    ASSERT_EQUAL_INT(0, memcmp(bmpImgRef.pxArray, bmpImgTest.pxArray, bmpImgRef.pxArraySize));

    free(bmpImgRef.pxArray);
    free(bmpImgTest.pxArray);
}

void testGrayscale() {
    size_t bufSizeTest;
    char *bufTest = readFile("res/parserTests/testRef.bmp", &bufSizeTest);
    if (bufTest == NULL) {
        FAIL("Failed to load test file");
        return;
    }

    uBMPImage bmpImgTest;
    if (bmpToArray_graysc(bufTest, bufSizeTest, &bmpImgTest)) {
        FAIL("Failed to parse test file to grayscale");
        free(bufTest);
        return;
    }

    uBMPImage bmpImgRef;
    if (bmpToArray(bufTest, bufSizeTest, &bmpImgRef)) {
        FAIL("Failed to parse test file to normal");
        free(bmpImgTest.pxArray);
        return;
    }
    free(bufTest);

    ASSERT_EQUAL_U_INT32(bmpImgRef.pxWidth, bmpImgTest.pxWidth);
    ASSERT_EQUAL_U_INT32(bmpImgRef.pxHeight, bmpImgTest.pxHeight);
    ASSERT_EQUAL_SIZE_T(bmpImgRef.pxArraySize / 3, bmpImgTest.pxArraySize);
    ASSERT_EQUAL_DOUBLE(bmpImgRef.pxArray[0]  *.0722 + bmpImgRef.pxArray[1]  *.7152 + bmpImgRef.pxArray[2]  *.2126, bmpImgTest.pxArray[0], 0.9999);
}

void runTestsParser(void) {
    START_TESTING();
    RUN_TEST(testFileError);
    RUN_TEST(testInvalidHeaderSignature);
    RUN_TEST(testTooSmallForHeader);
    RUN_TEST(testPicTooSmall);
    RUN_TEST(testWidthHeight);
    RUN_TEST(testNegativeWidthHeight);
    RUN_TEST(testGrayscale);
    STOP_TESTING();
}
