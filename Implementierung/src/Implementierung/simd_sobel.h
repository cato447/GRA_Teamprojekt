#include <stdint.h>
#include <stddef.h>
#include <immintrin.h>

#define COMP_255 255, 255, 255, 255, 255, 255, 255, 255
#define ZERO_EVEN_BYTES_MASK 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00

void simd_sobel(uint8_t* img_in, size_t width, size_t height, uint8_t* img_out);

void computeSIMDSobel(uint8_t* img_in, size_t i, size_t width, uint8_t* img_out, __m128i zeroEvenBytesMask, __m128i comparer);