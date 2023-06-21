#include "unittest.h"
#include <stdio.h>
#include <string.h>

struct TestResults testResults;

void startTesting(char* fileName) {
    testResults.testsRun = 0;
    testResults.testsPassed = 0;
    printf("--------------------\n");
    printf("%Running tests of %s\n", fileName);
}

void stopTesting() {
    printf("%d of %d Tests passed\n", testResults.testsPassed, testResults.testsRun);
}

void printSuccessMessage(const char *funcName, int fileNum) {
    printf("PASS %s:%d\n", funcName, fileNum);
}

void runTest(int (*f)()) {
    if (f() == 0) {
        testResults.testsPassed++;
    }
    testResults.testsRun++;
}

int unitAssert(bool condition, const char *funcName, int fileNum) {
    if (condition) {
        printSuccessMessage(funcName, fileNum);
	return 0;
    } else {
        printf("Fail %s:%d\n", funcName, fileNum);
	return 1;
    }
}

int unitAssertFalse(bool condition, const char *funcName, int fileNum) {
    if (condition) {
        printf("Fail %s:%d Expected false but was true\n", funcName, fileNum);
        return 1;
    } else {
        printSuccessMessage(funcName, fileNum);
        return 0;
    }
}

int unitAssertTrue(bool condition, const char *funcName, int fileNum) {
    if (condition) {
        printSuccessMessage(funcName, fileNum);
        return 0;
    } else {
        printf("FAIL %s:%d Expected true but was false\n", funcName, fileNum);
        return 1;
    }
}

int unitAssertEqualInt(const int expected, const int actual, const char *funcName, int fileNum) {
    if (expected != actual) {
        printf("FAIL %s:%d Expected %d but was %d\n", funcName, fileNum, expected, actual);
        return 1;
    } else {
        printSuccessMessage(funcName, fileNum);
        return 0;
    }
}

int unitAssertEqualInt8(const int8_t expected, const int8_t actual, const char *funcName, int fileNum) {
    if (expected != actual) {
        printf("FAIL %s:%d Expected %d but was %d\n", funcName, fileNum, expected, actual);
        return 1;
    } else {
        printSuccessMessage(funcName, fileNum);
        return 0;
    }
}

int unitAssertEqualUInt8(const uint8_t expected, const uint8_t actual, const char *funcName, int fileNum) {
    if (expected != actual) {
        printf("FAIL %s:%d Expected %u but was %u\n", funcName, fileNum, expected, actual);
        return 1;
    } else {
        printSuccessMessage(funcName, fileNum);
        return 0;
    }
}

int unitAssertEqualInt16(const int16_t expected, const int16_t actual, const char *funcName, int fileNum) {
    if (expected != actual) {
        printf("FAIL %s:%d Expected %d but was %d\n", funcName, fileNum, expected, actual);
        return 1;
    } else {
        printSuccessMessage(funcName, fileNum);
        return 0;
    }
}

int unitAssertEqualUInt16(const uint16_t expected, const uint16_t actual, const char *funcName, int fileNum) {
    if (expected != actual) {
        printf("FAIL %s:%d Expected %u but was %u\n", funcName, fileNum, expected, actual);
        return 1;
    } else {
        printSuccessMessage(funcName, fileNum);
        return 0;
    }
}

int unitAssertEqualInt32(const int32_t expected, const int32_t actual, const char *funcName, int fileNum) {
    if (expected != actual) {
        printf("FAIL %s:%d Expected %d but was %d\n", funcName, fileNum, expected, actual);
        return 1;
    } else {
        printSuccessMessage(funcName, fileNum);
        return 0;
    }
}

int unitAssertEqualUInt32(const uint32_t expected, const uint32_t actual, const char *funcName, int fileNum) {
    if (expected != actual) {
        printf("FAIL %s:%d Expected %u but was %u\n", funcName, fileNum, expected, actual);
        return 1;
    } else {
        printSuccessMessage(funcName, fileNum);
        return 0;
    }
}

int unitAssertEqualInt64(const int64_t expected, const int64_t actual, const char *funcName, int fileNum) {
    if (expected != actual) {
        printf("FAIL %s:%d Expected %" PRId64 " but was %" PRId64 "\n", funcName, fileNum, expected, actual);
        return 1;
    } else {
        printSuccessMessage(funcName, fileNum);
        return 0;
    }
}

int unitAssertEqualUInt64(const uint64_t expected, const uint64_t actual, const char *funcName, int fileNum) {
    if (expected != actual) {
        printf("FAIL %s:%d Expected %" PRIu64 " but was %" PRIu64 "\n", funcName, fileNum, expected, actual);
        return 1;
    } else {
        printSuccessMessage(funcName, fileNum);
        return 0;
    }
}

int unitAssertEqualString(const char *expected, const char *actual, const char *funcName, int fileNum) {
    if (strcmp(expected, actual) == 0) {
        printSuccessMessage(funcName, fileNum);
        return 0;
    } else {
        printf("FAIL %s:%d Expected \"%s\" but was \"%s\"\n", funcName, fileNum, expected, actual);
        return 1;
    }
}
