#include <stdint.h>
#include <stddef.h>
#include <immintrin.h>

//technically the same, this is just for the sake of readability
#define COMP_255 (uint16_t)255, (uint16_t)255, (uint16_t)255, (uint16_t)255, (uint16_t)255, (uint16_t)255, (uint16_t)255, (uint16_t)255
#define ZERO_EVEN_BYTES_MASK (uint16_t)0xFF, (uint16_t)0xFF, (uint16_t)0xFF, (uint16_t)0xFF, (uint16_t)0xFF, (uint16_t)0xFF, (uint16_t)0xFF, (uint16_t)0xFF

void simd_sobel(uint8_t *img_in, size_t width, size_t height, uint8_t *img_out);
void simd_sobel_graysc(uint8_t *img_in, size_t width, size_t height, uint8_t *img_out);

void computeSIMDSobel(uint8_t *img_in, size_t i, size_t width, uint8_t *img_out, __m128i zeroEvenBytesMask, __m128i comparer);
void computeSIMDSobel_graysc(uint8_t *img_in, size_t i, size_t width, uint8_t *img_out, __m128i zeroEvenBytesMask, __m128i comparer);