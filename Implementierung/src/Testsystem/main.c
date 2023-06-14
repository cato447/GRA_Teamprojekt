#include "unittest.h"

int testAssertEqualInt() {
    int i = 5;
    int j = 6;
    return unitAssertEqualInt(i, j, __FUNCTION__, __LINE__);
}

int testAssertEqualString() {
    char *string1 = "Hallo ich bin der Simon";
    char *string2 = "Hallo ich bin der Simon";
    return unitAssertEqualString(string1, string2, __FUNCTION__, __LINE__);
}

int testAssertEqualTrue() {
    bool condition = false;
    return unitAssertTrue(condition, __FUNCTION__, __LINE__);
}

int main(void) {
    startTesting();
    runTest(testAssertEqualInt);
    runTest(testAssertEqualString);
    runTest(testAssertEqualTrue);
    stopTesting();
}