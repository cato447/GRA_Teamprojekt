#include "unittest.h"

#include <stdio.h>
#include <string.h>

#define C_RED "\x1b[31m"
#define C_GREEN "\x1b[32m"
#define C_YELLOW "\x1b[33m"
#define C_BLUE "\x1b[34m"
#define C_RESET "\x1b[0m"

static FILE* tmp_stdout;
static FILE* tmp_stderr;

static struct TestResults {
    int testsRun;
    int testsPassed;
    int assertFailure;
} testResults;

void _unitStartTesting(char* fileName) {
    tmp_stdout = stdout;
    tmp_stderr = stderr;
    stdout = tmpfile();
    stderr = tmpfile();
    if (stdout == NULL || stderr == NULL) {
        stdout = tmp_stdout;
        stderr = tmp_stderr;
    }

    testResults.testsRun = 0;
    testResults.testsPassed = 0;
    testResults.assertFailure = false;
    fprintf(tmp_stdout, "\n╔══════════════════════════════════════════════════════════════════════════\n");
    fprintf(tmp_stdout, "║ " C_BLUE "Running tests of %s" C_RESET "\n", fileName);
    fprintf(tmp_stdout, "║\n");
}

void _unitStopTesting() {
    if (testResults.testsPassed == testResults.testsRun) {
        fprintf(tmp_stdout, "║\n");
        fprintf(tmp_stdout, "║ " C_GREEN "%d of %d Tests passed" C_RESET "\n", testResults.testsPassed, testResults.testsRun);
    } else if (testResults.testsPassed > (testResults.testsRun / 2)) {
        fprintf(tmp_stdout, "║\n");
        fprintf(tmp_stdout, "║ " C_YELLOW "%d of %d Tests passed" C_RESET "\n", testResults.testsPassed, testResults.testsRun);
    } else {
        fprintf(tmp_stdout, "║\n");
        fprintf(tmp_stdout, "║ " C_RED "%d of %d Tests passed" C_RESET "\n", testResults.testsPassed, testResults.testsRun);
    }

    char redir[150];

    if (stdout != tmp_stdout) {
        rewind(stdout);
        if (fgets(redir, sizeof(redir), stdout)) {
            fprintf(tmp_stdout, "║\n");
            fprintf(tmp_stdout, "║ " C_YELLOW "❯ Messages generated while running the tests:" C_RESET "\n");
            fprintf(tmp_stdout,"║ " C_YELLOW "   ▐ %s" C_RESET, redir);
            while (fgets(redir, sizeof(redir), stdout)) {
                fprintf(tmp_stdout, "║ " C_YELLOW "   ▐ %s" C_RESET, redir);
            }
        }
        fclose(stdout);
    }

    if (stderr != tmp_stderr) {
        rewind(stderr);
        if (fgets(redir, sizeof(redir), stderr)) {
            fprintf(tmp_stdout, "║\n");
            fprintf(tmp_stderr, "║ " C_RED "❯ Errors generated while running the tests:" C_RESET "\n");
            fprintf(tmp_stderr, "║ " C_RED "   ▐ %s" C_RESET, redir);
            while (fgets(redir, sizeof(redir), stderr)) {
                fprintf(tmp_stderr, "║ " C_RED "   ▐ %s" C_RESET, redir);
            }
        }
        fclose(stderr);
    }

    fprintf(tmp_stdout, "╚══════════════════════════════════════════════════════════════════════════\n");

    stdout = tmp_stdout;
    stderr = tmp_stderr;
}

void _unitRunTest(void (*f)(), const char* f_name) {
    testResults.assertFailure = false;
    fprintf(tmp_stdout, "║  ▗ ❯ %s\n", f_name);
    f();
    if (!testResults.assertFailure) {
        testResults.testsPassed++;
    }
    testResults.testsRun++;
}


static void assertPass(const char *funcName, int lineNum) {
    fprintf(tmp_stdout, "║  ▕" C_GREEN "     PASS %s:%d" C_RESET "\n", funcName, lineNum);
}
static void assertFailMsg(const char* funcName, int lineNum) {
    testResults.assertFailure = true;
    fprintf(tmp_stdout, "║  ▕" C_RED "     FAIL %s:%d  ▶  " C_RESET, funcName, lineNum);
}
static void assertFailNoMsg(const char* funcName, int lineNum) {
    testResults.assertFailure = true;
    fprintf(tmp_stdout, "║  ▕" C_RED "     FAIL %s:%d" C_RESET "\n", funcName, lineNum);
}


int _unitPass(const char *funcName, int lineNum) {
    assertPass(funcName, lineNum);
    return 0;
}

int _unitFail(char* message, const char *funcName, int lineNum) {
    assertFailMsg(funcName, lineNum);
    fprintf(tmp_stdout, "%s\n", message);
    return 1;
}

int _unitAssert(bool condition, const char *funcName, int lineNum) {
    if (condition) {
        assertPass(funcName, lineNum);
	return 0;
    } else {
        assertFailNoMsg(funcName, lineNum);
	    return 1;
    }
}

int _unitAsserEqualPointer(void* expected, void* actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailNoMsg(funcName, lineNum);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int _unitAssertFalse(bool condition, const char *funcName, int lineNum) {
    if (condition) {
        assertFailMsg(funcName, lineNum);
        fprintf(tmp_stdout, "Expected FALSE but was TRUE\n");
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int _unitAssertTrue(bool condition, const char *funcName, int lineNum) {
    if (condition) {
        assertPass(funcName, lineNum);
        return 0;
    } else {
        assertFailMsg(funcName, lineNum);
        fprintf(tmp_stdout, "Expected TRUE but was FALSE\n");
        return 1;
    }
}

int _unitAssertEqualSizeT(const size_t expected, const size_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        fprintf(tmp_stdout, "Expected %lu but was %lu\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int _unitAssertEqualInt(const int expected, const int actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        fprintf(tmp_stdout, "Expected %d but was %d\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int _unitAssertEqualInt8(const int8_t expected, const int8_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        fprintf(tmp_stdout, "Expected %d but was %d\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int _unitAssertEqualUInt8(const uint8_t expected, const uint8_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        fprintf(tmp_stdout, "Expected %u but was %u\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int _unitAssertEqualInt16(const int16_t expected, const int16_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        fprintf(tmp_stdout, "Expected %d but was %d\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int _unitAssertEqualUInt16(const uint16_t expected, const uint16_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        fprintf(tmp_stdout, "Expected %u but was %u\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int _unitAssertEqualInt32(const int32_t expected, const int32_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        fprintf(tmp_stdout, "Expected %d but was %d\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int _unitAssertEqualUInt32(const uint32_t expected, const uint32_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        fprintf(tmp_stdout, "Expected %u but was %u\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int _unitAssertEqualInt64(const int64_t expected, const int64_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        fprintf(tmp_stdout, "Expected %" PRId64 " but was %" PRId64 "\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int _unitAssertEqualUInt64(const uint64_t expected, const uint64_t actual, const char *funcName, int lineNum) {
    if (expected != actual) {
        assertFailMsg(funcName, lineNum);
        fprintf(tmp_stdout, "Expected %" PRIu64 " but was %" PRIu64 "\n", expected, actual);
        return 1;
    } else {
        assertPass(funcName, lineNum);
        return 0;
    }
}

int _unitAssertEqualString(const char *expected, const char *actual, const char *funcName, int lineNum) {
    if (strcmp(expected, actual) == 0) {
        assertPass(funcName, lineNum);
        return 0;
    } else {
        assertFailMsg(funcName, lineNum);
        fprintf(tmp_stdout, "Expected \"%s\" but was \"%s\"\n", expected, actual);
        return 1;
    }
}

int _unitAssertDouble(const double expected, const double actual, double tolerance, const char *funcName, int lineNum) {
    if (expected + tolerance > actual && expected - tolerance  < actual){
        assertPass(funcName, lineNum);
        return 0;
    } else {
        assertFailMsg(funcName, lineNum);
        fprintf(tmp_stdout, "Expected %f but was %f\n", expected, actual);
        return 1;
    }
}
