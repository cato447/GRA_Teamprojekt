#include "unittest.h"
#include <stdio.h>
#include <string.h>

#define C_RED "\x1b[31m"
#define C_GREEN "\x1b[32m"
#define C_YELLOW "\x1b[33m"
#define C_BLUE "\x1b[34m"
#define C_RESET "\x1b[0m"

struct TestResults testResults;

void startTesting(char* fileName) {
    testResults.testsRun = 0;
    testResults.testsPassed = 0;
    testResults.assertFailure = 0;
    printf("\n+─────────────────────────────────────────────────────────────────────\n");
    printf("│ " C_BLUE "Running tests of %s" C_RESET "\n│\n", fileName);
}

void stopTesting() {
    if (testResults.testsPassed == testResults.testsRun) {
        printf("│\n│ " C_GREEN "%d of %d Tests passed" C_RESET "\n", testResults.testsPassed, testResults.testsRun);
    } else if (testResults.testsPassed > (testResults.testsRun / 2)) {
        printf("│\n│ " C_YELLOW "%d of %d Tests passed" C_RESET "\n", testResults.testsPassed, testResults.testsRun);
    } else {
        printf("│\n│ " C_RED "%d of %d Tests passed" C_RESET "\n", testResults.testsPassed, testResults.testsRun);
    }
    printf("+─────────────────────────────────────────────────────────────────────\n\n");
}

static void assertPass(const char *funcName, int lineNum) {
    printf("│  █" C_GREEN "     PASS %s:%d" C_RESET "\n", funcName, lineNum);
}
static void assertFailMsg(const char* funcName, int lineNum) {
    testResults.assertFailure = true;
    printf("│  █" C_RED "     FAIL %s:%d  ▶  " C_RESET, funcName, lineNum);
}
static void assertFailNoMsg(const char* funcName, int lineNum) {
    testResults.assertFailure = true;
    printf("│  █" C_RED "     FAIL %s:%d" C_RESET "\n", funcName, lineNum);
}

void runTest(void (*f)(), const char* f_name) {
    testResults.assertFailure = false;
    printf("│  ▂ ❯ %s\n", f_name);
    f();
    if (!testResults.assertFailure) {
        testResults.testsPassed++;
    }
    testResults.testsRun++;
}

int unitFail(char* message, const char *funcName, int lineNum) {
    assertFailMsg(funcName, lineNum);
    printf("%s\n", message);
    return 1;
}

int unitAssert(bool condition, const char *funcName, int lineNum) {
    if (condition) {
        assertPass(funcName, lineNum);
	return 0;
    } else {
        assertFailNoMsg(funcName, lineNum);
	    return 1;
    }
}

int unitAssertFalse(bool condition, const char *funcName, int lineNum) {
    if (condition) {
        assertFailMsg(funcName, lineNum);
        printf("Expected FALSE but was TRUE\n");
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int unitAssertTrue(bool condition, const char *funcName, int lineNum) {
    if (condition) {
        assertPass(funcName, lineNum);
        return 0;
    } else {
        assertFailMsg(funcName, lineNum);
        printf("Expected TRUE but was FALSE\n");
        return 1;
    }
}

int unitAssertEqualInt(const int expected, const int actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        printf("Expected %d but was %d\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int unitAssertEqualInt8(const int8_t expected, const int8_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        printf("Expected %d but was %d\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int unitAssertEqualUInt8(const uint8_t expected, const uint8_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        printf("Expected %u but was %u\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int unitAssertEqualInt16(const int16_t expected, const int16_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        printf("Expected %d but was %d\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int unitAssertEqualUInt16(const uint16_t expected, const uint16_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        printf("Expected %u but was %u\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int unitAssertEqualInt32(const int32_t expected, const int32_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        printf("Expected %d but was %d\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int unitAssertEqualUInt32(const uint32_t expected, const uint32_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        printf("Expected %u but was %u\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int unitAssertEqualInt64(const int64_t expected, const int64_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        printf("Expected %" PRId64 " but was %" PRId64 "\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int unitAssertEqualUInt64(const uint64_t expected, const uint64_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        printf("Expected %" PRIu64 " but was %" PRIu64 "\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int unitAssertEqualString(const char *expected, const char *actual, const char *funcName, int lineNum) {
    if (strcmp(expected, actual) == 0) {
        assertPass(funcName, lineNum);
        return 0;
    } else {
        assertFailMsg(funcName, lineNum);
        printf("Expected \"%s\" but was \"%s\"\n", expected, actual);
        return 1;
    }
}

int unitAssertDouble(const double expected, const double actual, double tolerance, const char *funcName, int lineNum) {
    if (expected + tolerance > actual && expected - tolerance  < actual){
        assertPass(funcName, lineNum);
        return 0;
    } else {
        assertFailMsg(funcName, lineNum);
        printf("Expected %f but was %f\n", expected, actual);
        return 1;
    }
}
