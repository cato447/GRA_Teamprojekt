#include "unittest.h"

int testAssertEqualInt() {
    int i = 5;
    int j = 6;
    return ASSERT_EQUAL_INT(i, j);
}

int testAssertEqualString() {
    char *string1 = "Hallo ich bin der Simon";
    char *string2 = "Hallo ich bin der Simon";
    return ASSERT_EQUAL_STRING(string1, string2);
}

int testAssertEqualTrue() {
    bool condition = false;
    return ASSERT(condition);
}

/*
int main(void) {
    startTesting();
    runTest(testAssertEqualInt);
    runTest(testAssertEqualString);
    runTest(testAssertEqualTrue);
    stopTesting();
}
 */
