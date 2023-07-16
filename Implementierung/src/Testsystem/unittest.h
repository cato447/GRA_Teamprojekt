#ifndef _UNITTEST_H_
#define _UNITTEST_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// Controls
void _unit_start_testing(const char *fileName);
void _unit_stop_testing(bool print_messages);
void _unit_run_test(void (*f)(), const char *f_name);

// Assertions
int _unit_pass(const char *funcName, int lineNum);
int _unit_fail(const char *message, const char *funcName, int lineNum);
int _unit_assert(bool condition, const char *funcName, int lineNum);
int _unit_assert_false(bool condition, const char *funcName, int lineNum);
int _unit_assert_true(bool condition, const char *funcName, int lineNum);
int _unit_assert_equal_pointer(void *expected, void *actual, const char *funcName, int lineNum);
int _unit_assert_equal_size_t(const size_t expected, const size_t actual, const char *funcName, int lineNum);
int _unit_assert_equal_int(const int expected, const int actual, const char *funcName, int lineNum);
int _unit_assert_equal_int8(const int8_t expected, const int8_t actual, const char *funcName, int lineNum);
int _unit_assert_equal_uint8(const uint8_t expected, const uint8_t actual, const char *funcName, int lineNum);
int _unit_assert_equal_int16(const int16_t expected, const int16_t actual, const char *funcName, int lineNum);
int _unit_assert_equal_uint16(const uint16_t expected, const uint16_t actual, const char *funcName, int lineNum);
int _unit_assert_equal_int32(const int32_t expected, const int32_t actual, const char *funcName, int lineNum);
int _unit_assert_equal_uint32(const uint32_t expected, const uint32_t actual, const char *funcName, int lineNum);
int _unit_assert_equal_int64(const int64_t expected, const int64_t actual, const char *funcName, int lineNum);
int _unit_assert_equal_uint64(const uint64_t expected, const uint64_t actual, const char *funcName, int lineNum);
int _unit_assert_equal_string(const char *expected, const char *actual, const char *funcName, int lineNum);
int _unit_assert_equal_double(const double expected, const double actual, double tolerance, const char *funcName, int fileNum);


#define START_TESTING() _unit_start_testing(__BASE_FILE__)
#define STOP_TESTING_VERBOSE() _unit_stop_testing(true)
#define STOP_TESTING_QUIET() _unit_stop_testing(false)
#define RUN_TEST(F) _unit_run_test(F, #F "()")

#define PASS() _unit_pass(__func__, __LINE__)
#define FAIL(MESSAGE) _unit_fail(MESSAGE, __func__, __LINE__)
#define ASSERT(CONDITION) _unit_assert(CONDITION, __func__, __LINE__)
#define ASSERT_FALSE(CONDITION) _unit_assert_false(CONDITION, __func__, __LINE__)
#define ASSERT_TRUE(CONDITION) _unit_assert_true(CONDITION, __func__, __LINE__)
#define ASSERT_EQUAL_POINTER(EXPECTED, ACTUAL) _unit_assert_equal_pointer(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_SIZE_T(EXPECTED, ACTUAL) _unit_assert_equal_size_t(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_INT(EXPECTED, ACTUAL) _unit_assert_equal_int(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_INT8(EXPECTED, ACTUAL) _unit_assert_equal_int8(EXPECTED, ACTUAL, __func___, __LINE__)
#define ASSERT_EQUAL_UINT8(EXPECTED, ACTUAL) _unit_assert_equal_uint8(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_INT16(EXPECTED, ACTUAL) _unit_assert_equal_int16(EXPECTED, ACTUAL, __func___, __LINE__)
#define ASSERT_EQUAL_UINT16(EXPECTED, ACTUAL) _unit_assert_equal_uint16(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_INT32(EXPECTED, ACTUAL) _unit_assert_equal_int32(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_UINT32(EXPECTED, ACTUAL) _unit_assert_equal_uint32(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_INT64(EXPECTED, ACTUAL) _unit_assert_equal_int64(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_UINT64(EXPECTED, ACTUAL) _unit_assert_equal_uint64(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_STRING(EXPECTED, ACTUAL) _unit_assert_equal_string(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_DOUBLE(EXPECTED, ACTUAL, TOLERANCE) _unit_assert_equal_double(EXPECTED, ACTUAL, TOLERANCE, __func__, __LINE__)

#endif
