#include <stdint.h>
#include <stddef.h>
#include <immintrin.h>

extern const uint16_t COMP_255[];
extern const uint8_t ZERO_EVEN_BYTES_MASK[];
void simd_sobel(uint8_t* img_in, size_t width, size_t height, uint8_t* img_out);

__attribute__((__always_inline__, __hot__)) inline
void computeSIMDSobel(uint8_t* img_in, size_t i, size_t width, uint8_t* img_out, __m128i zeroEvenBytesMask, __m128i comparer);