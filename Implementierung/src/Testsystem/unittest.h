#ifndef UNITTEST_H_
#define UNITTEST_H_

#include <stdbool.h>
#include <stdint.h>

// Data
struct TestResults {
    int testsRun;
    int testsPassed;
};

// Controls

void startTesting();

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

#endif