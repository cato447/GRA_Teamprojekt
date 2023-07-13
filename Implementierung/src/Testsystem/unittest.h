#ifndef UNITTEST_H_
#define UNITTEST_H_

#include <stdbool.h>
#include <inttypes.h>

// Controls
void _unitStartTesting(char* fileName);
void _unitStopTesting();
void _unitRunTest(void (*f)(), const char* f_name);

// Assertions
int _unitPass(const char *funcName, int lineNum);
int _unitFail(char* message, const char *funcName, int lineNum);
int _unitAssert(bool condition, const char *funcName, int lineNum);
int _unitAssertFalse(bool condition, const char *funcName, int lineNum);
int _unitAssertTrue(bool condition, const char *funcName, int lineNum);
int _unitAssertEqualInt(const int expected, const int actual, const char *funcName, int lineNum);
int _unitAssertEqualInt8(const int8_t expected, const int8_t actual, const char *funcName, int lineNum);
int _unitAssertEqualUInt8(const uint8_t expected, const uint8_t actual, const char *funcName, int lineNum);
int _unitAssertEqualInt16(const int16_t expected, const int16_t actual, const char *funcName, int lineNum);
int _unitAssertEqualUInt16(const uint16_t expected, const uint16_t actual, const char *funcName, int lineNum);
int _unitAssertEqualInt32(const int32_t expected, const int32_t actual, const char *funcName, int lineNum);
int _unitAssertEqualUInt32(const uint32_t expected, const uint32_t actual, const char *funcName, int lineNum);
int _unitAssertEqualInt64(const int64_t expected, const int64_t actual, const char *funcName, int lineNum);
int _unitAssertEqualUInt64(const uint64_t expected, const uint64_t actual, const char *funcName, int lineNum);
int _unitAssertEqualString(const char *expected, const char *actual, const char *funcName, int lineNum);
int _unitAssertDouble(const double expected, const double actual, double tolerance, const char *funcName, int fileNum);


#define START_TESTING _unitStartTesting(__BASE_FILE__)
#define STOP_TESTING _unitStopTesting()
#define RUN_TEST(F) _unitRunTest(F, #F "()")

#define PASS() _unitPass(__func__, __LINE__)
#define FAIL(MESSAGE) _unitFail(MESSAGE, __func__, __LINE__)
#define ASSERT(CONDITION) _unitAssert(CONDITION, __func__, __LINE__)
#define ASSERT_FALSE(CONDITION) _unitAssertFalse(CONDITION, __func__, __LINE__)
#define ASSERT_TRUE(CONDITION) _unitAssertTrue(CONDITION, __func__, __LINE__)
#define ASSERT_EQUAL_INT(EXPECTED, ACTUAL) _unitAssertEqualInt(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_INT8(EXPECTED, ACTUAL) _unitAssertEqualInt8(EXPECTED, ACTUAL, __func___, __LINE__)
#define ASSERT_EQUAL_U_INT8(EXPECTED, ACTUAL) _unitAssertEqualUInt8(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_INT16(EXPECTED, ACTUAL) _unitAssertEqualInt16(EXPECTED, ACTUAL, __func___, __LINE__)
#define ASSERT_EQUAL_U_INT16(EXPECTED, ACTUAL) _unitAssertEqualUInt16(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_INT32(EXPECTED, ACTUAL) _unitAssertEqualInt32(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_U_INT32(EXPECTED, ACTUAL) _unitAssertEqualUInt32(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_INT64(EXPECTED, ACTUAL) _unitAssertEqualInt64(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_U_INT64(EXPECTED, ACTUAL) _unitAssertEqualUInt64(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_STRING(EXPECTED, ACTUAL) _unitAssertEqualString(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_DOUBLE(EXPECTED, ACTUAL, TOLERANCE) _unitAssertDouble(EXPECTED, ACTUAL, TOLERANCE, __func__, __LINE__)

#endif
