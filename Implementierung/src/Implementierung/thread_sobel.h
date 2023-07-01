#include <stdint.h>
#include <stddef.h>

void thread_sobel(uint8_t* img_in, size_t width, size_t height, uint8_t* img_out);
uint8_t* pointerToCoordinates(uint8_t* img_start, size_t x,  size_t y, size_t width);
void* computeSobelForHeightInterval(void* args);

typedef struct {
    uint8_t* img_in;
    size_t width;
    size_t fromY;
    size_t toY;
    uint8_t* img_out;
} sobelIntervalArgs;