#include <stdint.h>
#include <stddef.h>

extern const uint8_t SHIFT_BY2_MASK[];
extern const uint8_t SHIFT_BY1_MASK[];

void simd_sobel(uint8_t* img_in, size_t width, size_t height, uint8_t* img_out);
uint8_t* pointerToCoordinates(uint8_t* img_start, size_t x,  size_t y, size_t width);
