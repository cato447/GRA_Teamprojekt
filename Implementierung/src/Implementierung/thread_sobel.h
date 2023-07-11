#include <stdint.h>
#include <stddef.h>

void thread_sobel(uint8_t* img_in, size_t width, size_t height, uint8_t* img_out);
void* computeSobelForHeightInterval(void* args);

typedef struct {
    uint8_t* img_in;
    size_t width;
    size_t fromY;
    size_t toY;
    uint8_t* img_out;
} sobelIntervalArgs;