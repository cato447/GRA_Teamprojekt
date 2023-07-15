#include "test_IO_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Testsystem/unittest.h"
#include "IO_tools.h"
#include "bmp_parser.h"

void parse_failure(const char *path) {
    size_t buf_size;
    char *buf = read_file(path, &buf_size);
    if (buf == NULL) {
        FAIL("Failed to load test file");
        return;
    }
    s_image bmp_img;
    if (bmp_to_array(buf, buf_size, &bmp_img)) {
        PASS();
    } else {
        FAIL("Managed to parse invalid file");
        free(bmp_img.px_array);
    }

    free(buf);
}

void test_file_error(void) {
    size_t buf_size;
    char *buf = read_file("/mostLikelyDoesntExist", &buf_size);
    if (buf != NULL) {
        FAIL("Managed to read non-existing file");
        free(buf);
    } else {
        PASS();
    }
    buf = read_file("/", &buf_size);
    if (buf != NULL) {
        FAIL("Managed to read invalid file");
        free(buf);
    } else {
        PASS();
    }
}

void test_invalid_header_signature(void) {
    parse_failure("res/parserTests/invalidHeaderSignature.bmp");
}

void test_too_small_for_header(void) {
    parse_failure("res/parserTests/tooSmallForHeader.bmp");
}

void test_pic_too_small(void) {
    parse_failure("res/parserTests/picTooSmall.bmp");
}

void test_width_height(void) {
    size_t buf_size;
    char *buf = read_file("res/parserTests/testWidthHeight.bmp", &buf_size);
    if (buf == NULL) {
        FAIL("Failed to load test file");
        return;
    }

    s_image bmp_img;
    if (bmp_to_array(buf, buf_size, &bmp_img)) {
        FAIL("Failed to parse test file");
        free(buf);
        return;
    }
    free(buf);

    ASSERT_EQUAL_UINT32(199, bmp_img.px_width);
    ASSERT_EQUAL_UINT32(323, bmp_img.px_height);
    ASSERT_EQUAL_SIZE_T(199  *323  *3, bmp_img.px_array_size);

    free(bmp_img.px_array);
}

void test_negative_width_height(void) {
    size_t buf_size_test;
    char *buf_test = read_file("res/parserTests/negativeHeight.bmp", &buf_size_test);
    if (buf_test == NULL) {
        FAIL("Failed to load test file");
        return;
    }

    size_t buf_size_ref;
    char *buf_ref = read_file("res/parserTests/negativeHeightRef.bmp", &buf_size_ref);
    if (buf_ref == NULL) {
        free(buf_test);
        FAIL("Failed to load reference file");
        return;
    }

    s_image bmp_img_test;
    if (bmp_to_array(buf_test, buf_size_test, &bmp_img_test)) {
        FAIL("Failed to parse test file");
        free(buf_test);
        free(buf_ref);
        return;
    }
    free(buf_test);

    

    s_image bmp_img_ref;
    if (bmp_to_array(buf_ref, buf_size_ref, &bmp_img_ref)) {
        FAIL("Failed to parse reference file");
        free(bmp_img_test.px_array);
        free(buf_ref);
        return;
    }
    free(buf_ref);

    ASSERT_EQUAL_UINT32(bmp_img_ref.px_width, bmp_img_test.px_width);
    ASSERT_EQUAL_UINT32(bmp_img_ref.px_height, bmp_img_test.px_height);

    if (ASSERT_EQUAL_SIZE_T(bmp_img_ref.px_array_size, bmp_img_test.px_array_size)) {
        free(bmp_img_ref.px_array);
        free(bmp_img_test.px_array);
        return;
    }

    ASSERT_EQUAL_INT(0, memcmp(bmp_img_ref.px_array, bmp_img_test.px_array, bmp_img_ref.px_array_size));

    free(bmp_img_ref.px_array);
    free(bmp_img_test.px_array);
}

void test_grayscale(void) {
    size_t buf_size_test;
    char *buf_test = read_file("res/parserTests/testRef.bmp", &buf_size_test);
    if (buf_test == NULL) {
        FAIL("Failed to load test file");
        return;
    }

    s_image bmp_img_test;
    if (bmp_to_array_graysc(buf_test, buf_size_test, &bmp_img_test)) {
        FAIL("Failed to parse test file to grayscale");
        free(buf_test);
        return;
    }

    s_image bmp_img_ref;
    if (bmp_to_array(buf_test, buf_size_test, &bmp_img_ref)) {
        FAIL("Failed to parse test file to normal");
        free(bmp_img_test.px_array);
        return;
    }
    free(buf_test);

    ASSERT_EQUAL_UINT32(bmp_img_ref.px_width, bmp_img_test.px_width);
    ASSERT_EQUAL_UINT32(bmp_img_ref.px_height, bmp_img_test.px_height);
    ASSERT_EQUAL_SIZE_T(bmp_img_ref.px_array_size / 3, bmp_img_test.px_array_size);
    ASSERT_EQUAL_DOUBLE(bmp_img_ref.px_array[0]  *.0722 + bmp_img_ref.px_array[1]  *.7152 + bmp_img_ref.px_array[2]  *.2126, bmp_img_test.px_array[0], 0.9999);
}

void run_tests_parser(void) {
    START_TESTING();
    RUN_TEST(test_file_error);
    RUN_TEST(test_invalid_header_signature);
    RUN_TEST(test_too_small_for_header);
    RUN_TEST(test_pic_too_small);
    RUN_TEST(test_width_height);
    RUN_TEST(test_negative_width_height);
    RUN_TEST(test_grayscale);
    STOP_TESTING();
}
