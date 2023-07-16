#include <stdint.h>
#include <stddef.h>
#include <immintrin.h>

//technically the same, this is just for the sake of readability
#define COMP_255 (uint16_t)255, (uint16_t)255, (uint16_t)255, (uint16_t)255, (uint16_t)255, (uint16_t)255, (uint16_t)255, (uint16_t)255
#define ZERO_EVEN_BYTES_MASK (uint16_t)0xFF, (uint16_t)0xFF, (uint16_t)0xFF, (uint16_t)0xFF, (uint16_t)0xFF, (uint16_t)0xFF, (uint16_t)0xFF, (uint16_t)0xFF

void sobel_simd(const uint8_t *img_in, size_t width, size_t height, uint8_t *img_out);
void sobel_simd_graysc(const uint8_t *img_in, size_t width, size_t height, uint8_t *img_out);

void compute_sobel_simd(const uint8_t *img_in, size_t i, size_t width, uint8_t *img_out, __m128i zero_even_bytes_mask, __m128i comparer);
void compute_sobel_simd_graysc(const uint8_t *img_in, size_t i, size_t width, uint8_t *img_out, __m128i zero_even_bytes_mask, __m128i comparer);
