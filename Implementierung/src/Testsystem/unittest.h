#ifndef UNITTEST_H_
#define UNITTEST_H_

#include <stdbool.h>
#include <inttypes.h>

// Data
struct TestResults {
    int testsRun;
    int testsPassed;
};

// Controls

void startTesting(char* fileName);

void stopTesting();

void runTest(int (*f)());

// Assertions

int unitAssert(bool condition, const char *funcName, int lineNum);

int unitAssertFalse(bool condition, const char *funcName, int lineNum);

int unitAssertTrue(bool condition, const char *funcName, int lineNum);

int unitAssertEqualInt(const int expected, const int actual, const char *funcName, int lineNum);

int unitAssertEqualInt8(const int8_t expected, const int8_t actual, const char *funcName, int lineNum);

int unitAssertEqualUInt8(const uint8_t expected, const uint8_t actual, const char *funcName, int lineNum);

int unitAssertEqualInt16(const int16_t expected, const int16_t actual, const char *funcName, int lineNum);

int unitAssertEqualUInt16(const uint16_t expected, const uint16_t actual, const char *funcName, int lineNum);

int unitAssertEqualInt32(const int32_t expected, const int32_t actual, const char *funcName, int lineNum);

int unitAssertEqualUInt32(const uint32_t expected, const uint32_t actual, const char *funcName, int lineNum);

int unitAssertEqualInt64(const int64_t expected, const int64_t actual, const char *funcName, int lineNum);

int unitAssertEqualUInt64(const uint64_t expected, const uint64_t actual, const char *funcName, int lineNum);

int unitAssertEqualString(const char *expected, const char *actual, const char *funcName, int lineNum);


#define ASSERT(CONDITION) unitAssert(CONDITION, __func__, __LINE__)
#define ASSERT_FALSE(CONDITION) unitAssertFalse(CONDITION, __func__, __LINE__)
#define ASSERT_TRUE(CONDITION) unitAssertTrue(CONDITION, __func__, __LINE__)
#define ASSERT_EQUAL_INT(EXPECTED, ACTUAL) unitAssertEqualInt(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_INT8(EXPECTED, ACTUAL) unitAssertEqualInt8(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_U_INT8(EXPECTED, ACTUAL) unitAssertEqualUInt8(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_INT16(EXPECTED, ACTUAL) unitAssertEqualInt16(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_U_INT16(EXPECTED, ACTUAL) unitAssertEqualUInt16(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_INT32(EXPECTED, ACTUAL) unitAssertEqualInt32(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_U_INT32(EXPECTED, ACTUAL) unitAssertEqualUInt32(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_INT64(EXPECTED, ACTUAL) unitAssertEqualInt64(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_U_INT64(EXPECTED, ACTUAL) unitAssertEqualUInt64(EXPECTED, ACTUAL, __func__, __LINE__)
#define ASSERT_EQUAL_STRING(EXPECTED, ACTUAL) unitAssertEqualString(EXPECTED, ACTUAL, __func__, __LINE__)


#endif
