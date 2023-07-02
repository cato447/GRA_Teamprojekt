#include <stdint.h>
#include <stddef.h>

extern const uint16_t COMP_255[];
extern const uint8_t ZERO_EVEN_BYTES_MASK[];
void simd_sobel(uint8_t* img_in, size_t width, size_t height, uint8_t* img_out);