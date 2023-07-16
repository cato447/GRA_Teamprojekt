#include "unittest.h"

#include <stdio.h>
#include <string.h>

#define C_RED "\x1b[31m"
#define C_GREEN "\x1b[32m"
#define C_YELLOW "\x1b[33m"
#define C_BLUE "\x1b[34m"
#define C_RESET "\x1b[0m"

static FILE *tmp_stdout;
static FILE *tmp_stderr;

static struct test_results {
    int tests_run;
    int tests_passed;
    int assert_Failure;
} testResults;

void _unit_start_testing(const char *fileName) {
    tmp_stdout = stdout;
    tmp_stderr = stderr;
    stdout = tmpfile();
    stderr = tmpfile();
    if (stdout == NULL || stderr == NULL) {
        stdout = tmp_stdout;
        stderr = tmp_stderr;
    }

    testResults.tests_run = 0;
    testResults.tests_passed = 0;
    testResults.assert_Failure = false;
    fprintf(tmp_stdout, "╔══════════════════════════════════════════════════════════════════════════\n");
    fprintf(tmp_stdout, "║ " C_BLUE "Running tests of %s" C_RESET "\n", fileName);
    fprintf(tmp_stdout, "║\n");
}

void _unit_stop_testing(bool print_messages) {
    if (testResults.tests_passed == testResults.tests_run) {
        fprintf(tmp_stdout, "║\n");
        fprintf(tmp_stdout, "║ " C_GREEN "%d of %d Tests passed" C_RESET "\n", testResults.tests_passed, testResults.tests_run);
    } else if (testResults.tests_passed > (testResults.tests_run / 2)) {
        fprintf(tmp_stdout, "║\n");
        fprintf(tmp_stdout, "║ " C_YELLOW "%d of %d Tests passed" C_RESET "\n", testResults.tests_passed, testResults.tests_run);
    } else {
        fprintf(tmp_stdout, "║\n");
        fprintf(tmp_stdout, "║ " C_RED "%d of %d Tests passed" C_RESET "\n", testResults.tests_passed, testResults.tests_run);
    }


    if (stdout != tmp_stdout) {
        if (print_messages) {
            char redir[150];
            rewind(stdout);
            if (fgets(redir, sizeof(redir), stdout)) {
                fprintf(tmp_stdout, "║\n");
                fprintf(tmp_stdout, "║ " C_YELLOW "❯ Messages generated while running the tests:" C_RESET "\n");
                fprintf(tmp_stdout,"║ " C_YELLOW "   ▐ %s" C_RESET, redir);
                while (fgets(redir, sizeof(redir), stdout)) {
                    fprintf(tmp_stdout, "║ " C_YELLOW "   ▐ %s" C_RESET, redir);
                }
            }
        }
        fclose(stdout);
    }

    if (stderr != tmp_stderr) {
        if (print_messages) {
            char redir[150];
            rewind(stderr);
            if (fgets(redir, sizeof(redir), stderr)) {
                fprintf(tmp_stdout, "║\n");
                fprintf(tmp_stderr, "║ " C_RED "❯ Errors generated while running the tests:" C_RESET "\n");
                fprintf(tmp_stderr, "║ " C_RED "   ▐ %s" C_RESET, redir);
                while (fgets(redir, sizeof(redir), stderr)) {
                    fprintf(tmp_stderr, "║ " C_RED "   ▐ %s" C_RESET, redir);
                }
            }
        }
        fclose(stderr);
    }

    fprintf(tmp_stdout, "╚══════════════════════════════════════════════════════════════════════════\n");
    fprintf(tmp_stdout, "\n");

    stdout = tmp_stdout;
    stderr = tmp_stderr;
}

void _unit_run_test(void (*f)(), const char *f_name) {
    testResults.assert_Failure = false;
    fprintf(tmp_stdout, "║  ▗ ❯ %s\n", f_name);
    f();
    if (!testResults.assert_Failure) {
        testResults.tests_passed++;
    }
    testResults.tests_run++;
}


static void assert_pass(const char *func_name, int line_num) {
    fprintf(tmp_stdout, "║  ▕" C_GREEN "     PASS %s:%d" C_RESET "\n", func_name, line_num);
}
static void assert_fail_msg(const char *func_name, int line_num) {
    testResults.assert_Failure = true;
    fprintf(tmp_stdout, "║  ▕" C_RED "     FAIL %s:%d  ▶  " C_RESET, func_name, line_num);
}
static void assert_fail_no_msg(const char *func_name, int line_num) {
    testResults.assert_Failure = true;
    fprintf(tmp_stdout, "║  ▕" C_RED "     FAIL %s:%d" C_RESET "\n", func_name, line_num);
}


int _unit_pass(const char *func_name, int line_num) {
    assert_pass(func_name, line_num);
    return 0;
}

int _unit_fail(const char *message, const char *func_name, int line_num) {
    assert_fail_msg(func_name, line_num);
    fprintf(tmp_stdout, "%s\n", message);
    return 1;
}

int _unit_assert(bool condition, const char *func_name, int line_num) {
    if (condition) {
        assert_pass(func_name, line_num);
	return 0;
    } else {
        assert_fail_no_msg(func_name, line_num);
	    return 1;
    }
}

int _unit_assert_equal_pointer(void *expected, void *actual, const char *func_name, int line_num) {
    if (expected != actual) {
        assert_fail_no_msg(func_name, line_num);
        return 1;
    } else {
        assert_pass(func_name, line_num);
        return 0;
    }
}

int _unit_assert_false(bool condition, const char *func_name, int line_num) {
    if (condition) {
        assert_fail_msg(func_name, line_num);
        fprintf(tmp_stdout, "Expected FALSE but was TRUE\n");
        return 1;
    } else {
        assert_pass(func_name, line_num);
        return 0;
    }
}

int _unit_assert_true(bool condition, const char *func_name, int line_num) {
    if (condition) {
        assert_pass(func_name, line_num);
        return 0;
    } else {
        assert_fail_msg(func_name, line_num);
        fprintf(tmp_stdout, "Expected TRUE but was FALSE\n");
        return 1;
    }
}

int _unit_assert_equal_size_t(const size_t expected, const size_t actual, const char *func_name, int line_num) {
    if (expected != actual) {
        assert_fail_msg(func_name, line_num);
        fprintf(tmp_stdout, "Expected %lu but was %lu\n", expected, actual);
        return 1;
    } else {
        assert_pass(func_name, line_num);
        return 0;
    }
}

int _unit_assert_equal_int(const int expected, const int actual, const char *func_name, int line_num) {
    if (expected != actual) {
        assert_fail_msg(func_name, line_num);
        fprintf(tmp_stdout, "Expected %d but was %d\n", expected, actual);
        return 1;
    } else {
        assert_pass(func_name, line_num);
        return 0;
    }
}

int _unit_assert_equal_int8(const int8_t expected, const int8_t actual, const char *func_name, int line_num) {
    if (expected != actual) {
        assert_fail_msg(func_name, line_num);
        fprintf(tmp_stdout, "Expected %d but was %d\n", expected, actual);
        return 1;
    } else {
        assert_pass(func_name, line_num);
        return 0;
    }
}

int _unit_assert_equal_uint8(const uint8_t expected, const uint8_t actual, const char *func_name, int line_num) {
    if (expected != actual) {
        assert_fail_msg(func_name, line_num);
        fprintf(tmp_stdout, "Expected %u but was %u\n", expected, actual);
        return 1;
    } else {
        assert_pass(func_name, line_num);
        return 0;
    }
}

int _unit_assert_equal_int16(const int16_t expected, const int16_t actual, const char *func_name, int line_num) {
    if (expected != actual) {
        assert_fail_msg(func_name, line_num);
        fprintf(tmp_stdout, "Expected %d but was %d\n", expected, actual);
        return 1;
    } else {
        assert_pass(func_name, line_num);
        return 0;
    }
}

int _unit_assert_equal_uint16(const uint16_t expected, const uint16_t actual, const char *func_name, int line_num) {
    if (expected != actual) {
        assert_fail_msg(func_name, line_num);
        fprintf(tmp_stdout, "Expected %u but was %u\n", expected, actual);
        return 1;
    } else {
        assert_pass(func_name, line_num);
        return 0;
    }
}

int _unit_assert_equal_int32(const int32_t expected, const int32_t actual, const char *func_name, int line_num) {
    if (expected != actual) {
        assert_fail_msg(func_name, line_num);
        fprintf(tmp_stdout, "Expected %d but was %d\n", expected, actual);
        return 1;
    } else {
        assert_pass(func_name, line_num);
        return 0;
    }
}

int _unit_assert_equal_uint32(const uint32_t expected, const uint32_t actual, const char *func_name, int line_num) {
    if (expected != actual) {
        assert_fail_msg(func_name, line_num);
        fprintf(tmp_stdout, "Expected %u but was %u\n", expected, actual);
        return 1;
    } else {
        assert_pass(func_name, line_num);
        return 0;
    }
}

int _unit_assert_equal_int64(const int64_t expected, const int64_t actual, const char *func_name, int line_num) {
    if (expected != actual) {
        assert_fail_msg(func_name, line_num);
        fprintf(tmp_stdout, "Expected %ld but was %ld \n", expected, actual);
        return 1;
    } else {
        assert_pass(func_name, line_num);
        return 0;
    }
}

int _unit_assert_equal_uint64(const uint64_t expected, const uint64_t actual, const char *func_name, int line_num) {
    if (expected != actual) {
        assert_fail_msg(func_name, line_num);
        fprintf(tmp_stdout, "Expected %lu but was %lu\n", expected, actual);
        return 1;
    } else {
        assert_pass(func_name, line_num);
        return 0;
    }
}

int _unit_assert_equal_string(const char *expected, const char *actual, const char *func_name, int line_num) {
    if (strcmp(expected, actual) == 0) {
        assert_pass(func_name, line_num);
        return 0;
    } else {
        assert_fail_msg(func_name, line_num);
        fprintf(tmp_stdout, "Expected \"%s\" but was \"%s\"\n", expected, actual);
        return 1;
    }
}

int _unit_assert_equal_double(const double expected, const double actual, double tolerance, const char *func_name, int line_num) {
    if (expected + tolerance > actual && expected - tolerance  < actual){
        assert_pass(func_name, line_num);
        return 0;
    } else {
        assert_fail_msg(func_name, line_num);
        fprintf(tmp_stdout, "Expected %.9f but was %.9f\n", expected, actual);
        return 1;
    }
}
