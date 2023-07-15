#include "simd_sobel.h"

#include <immintrin.h>

#include "basic_sobel.h"

void simd_sobel(uint8_t *img_in, size_t width, size_t height, uint8_t *img_out) {
    if (width * 3 * height >= 16 * 3 + 3 + 3 && height >= 3) {
        __m128i zero_even_bytes_mask = _mm_set_epi16(ZERO_EVEN_BYTES_MASK);
        __m128i comparer = _mm_set_epi16(COMP_255);

        size_t i = width * 3 + 3;

        for (; i < width * (height - 1) * 3 - 3 - 16; i += 16) {
            compute_simd_sobel(img_in, i, width, img_out, zero_even_bytes_mask, comparer);
        }

        for (; i < width * (height - 1) * 3 - 3; ++i) {
            uint8_t upper_left = *(img_in + i - width * 3 - 3);
            uint8_t upper = *(img_in + i - width * 3);
            uint8_t upper_right = *(img_in + i - width * 3 + 3);
            uint8_t left = *(img_in + i - 3);
            uint8_t right = *(img_in + i + 3);
            uint8_t lower_left = *(img_in + i + width * 3 - 3);
            uint8_t lower = *(img_in + i + width * 3);
            uint8_t lower_right = *(img_in + i + width * 3 + 3);

            int32_t A_v = upper_left - lower_left + 2 * upper - 2 * lower + upper_right - lower_right;
            int32_t A_h = upper_left - upper_right + 2 * left - 2 * right + lower_left - lower_right;

            *(img_out + i) = abs(A_v) + abs(A_h) > 255 ? 255 : abs(A_v) + abs(A_h);
        }
        for (size_t y = 0; y < height; y++) {
            set_pixel_at(img_out, width, 0, y, 0, 0, 0);
            set_pixel_at(img_out, width, width-1, y, 0, 0, 0);
        }
    } else {
        sobel(img_in, width, height, img_out);
    }
}

void simd_sobel_graysc(uint8_t *img_in, size_t width, size_t height, uint8_t *img_out) {
    if (width * height >= 16 + 1 + 1 && height >= 1) {
        __m128i zero_even_bytes_mask = _mm_set_epi16(ZERO_EVEN_BYTES_MASK);
        __m128i comparer = _mm_set_epi16(COMP_255);

        size_t i = width + 1;

        for (; i < width * (height - 1) - 1 - 16; i += 16) {
            compute_simd_sobel_graysc(img_in, i, width, img_out, zero_even_bytes_mask, comparer);
        }

        for (; i < width * (height - 1) - 1; ++i) {
            uint8_t upper_left = *(img_in + i - width - 1);
            uint8_t upper = *(img_in + i - width);
            uint8_t upper_right = *(img_in + i - width + 1);
            uint8_t left = *(img_in + i - 1);
            uint8_t right = *(img_in + i + 1);
            uint8_t lower_left = *(img_in + i + width - 1);
            uint8_t lower = *(img_in + i + width);
            uint8_t lower_right = *(img_in + i + width + 1);

            int32_t A_v = upper_left - lower_left + 2 * upper - 2 * lower + upper_right - lower_right;
            int32_t A_h = upper_left - upper_right + 2 * left - 2 * right + lower_left - lower_right;

            *(img_out + i) = abs(A_v) + abs(A_h) > 255 ? 255 : abs(A_v) + abs(A_h);
        }
        for (size_t y = 0; y < height; y++) {
            img_out[y * width] = 0;
            img_out[y * width + width - 1] = 0;
        }
    } else {
        sobel_graysc(img_in, width, height, img_out);
    }
}

__m128i applyFilter(__m128i upper_left, __m128i upper, __m128i upper_right, __m128i left, __m128i right, __m128i lower_left, __m128i lower, __m128i lower_right, __m128i comparer, __m128i zero_even_bytes_mask);

__attribute__((__always_inline__, __hot__)) inline
void compute_simd_sobel(uint8_t *img_in, size_t i, size_t width, uint8_t *img_out, __m128i zero_even_bytes_mask,
                      __m128i comparer) {
    __m128i upper_left = _mm_loadu_si128((const __m128i *) (img_in + i - width * 3 - 3));
    __m128i upper = _mm_loadu_si128((const __m128i *) (img_in + i - width * 3));
    __m128i upper_right = _mm_loadu_si128((const __m128i *) (img_in + i - width * 3 + 3));
    __m128i left = _mm_loadu_si128((const __m128i *) (img_in + i - 3));
    __m128i right = _mm_loadu_si128((const __m128i *) (img_in + i + 3));
    __m128i lower_left = _mm_loadu_si128((const __m128i *) (img_in + i + width * 3 - 3));
    __m128i lower = _mm_loadu_si128((const __m128i *) (img_in + i + width * 3));
    __m128i lower_right = _mm_loadu_si128((const __m128i *) (img_in + i + width * 3 + 3));

    __m128i upper_left_2 = _mm_loadu_si128((const __m128i *) (img_in + i - width * 3 - 3 + 1));
    __m128i upper_2 = _mm_loadu_si128((const __m128i *) (img_in + i - width * 3 + 1));
    __m128i upper_right_2 = _mm_loadu_si128((const __m128i *) (img_in + i - width * 3 + 3 + 1));
    __m128i left_2 = _mm_loadu_si128((const __m128i *) (img_in + i - 3 + 1));
    __m128i right_2 = _mm_loadu_si128((const __m128i *) (img_in + i + 3 + 1));
    __m128i lower_left_2 = _mm_loadu_si128((const __m128i *) (img_in + i + width * 3 - 3 + 1));
    __m128i lower_2 = _mm_loadu_si128((const __m128i *) (img_in + i + width * 3 + 1));
    __m128i lower_right_2 = _mm_loadu_si128((const __m128i *) (img_in + i + width * 3 + 3 + 1));

    __m128i result = applyFilter(upper_left, upper, upper_right, left, right, lower_left, lower, lower_right, comparer, zero_even_bytes_mask);
    __m128i result_2 = applyFilter(upper_left_2, upper_2, upper_right_2, left_2, right_2, lower_left_2, lower_2, lower_right_2, comparer, zero_even_bytes_mask);

    //Shift the bits from addr + 1 in place
    result_2 = _mm_slli_epi16(result_2, 8);

    _mm_storeu_si128((__m128i *) (img_out + i), _mm_or_si128(result_2, result));
}


__attribute__((__always_inline__, __hot__)) inline
void compute_simd_sobel_graysc(uint8_t *img_in, size_t i, size_t width, uint8_t *img_out, __m128i zero_even_bytes_mask,
                      __m128i comparer) {
    __m128i upper_left = _mm_loadu_si128((const __m128i *) (img_in + i - width - 1));
    __m128i upper = _mm_loadu_si128((const __m128i *) (img_in + i - width));
    __m128i upper_right = _mm_loadu_si128((const __m128i *) (img_in + i - width + 1));
    __m128i left = _mm_loadu_si128((const __m128i *) (img_in + i - 1));
    __m128i right = _mm_loadu_si128((const __m128i *) (img_in + i + 1));
    __m128i lower_left = _mm_loadu_si128((const __m128i *) (img_in + i + width - 1));
    __m128i lower = _mm_loadu_si128((const __m128i *) (img_in + i + width));
    __m128i lower_right = _mm_loadu_si128((const __m128i *) (img_in + i + width + 1));

    __m128i upper_left_2 = _mm_loadu_si128((const __m128i *) (img_in + i - width - 1 + 1));
    __m128i upper_2 = _mm_loadu_si128((const __m128i *) (img_in + i - width + 1));
    __m128i upper_right_2 = _mm_loadu_si128((const __m128i *) (img_in + i - width + 1 + 1));
    __m128i left_2 = _mm_loadu_si128((const __m128i *) (img_in + i - 1 + 1));
    __m128i right_2 = _mm_loadu_si128((const __m128i *) (img_in + i + 1 + 1));
    __m128i lower_left_2 = _mm_loadu_si128((const __m128i *) (img_in + i + width - 1 + 1));
    __m128i lower_2 = _mm_loadu_si128((const __m128i *) (img_in + i + width + 1));
    __m128i lower_right_2 = _mm_loadu_si128((const __m128i *) (img_in + i + width + 1 + 1));

    __m128i result = applyFilter(upper_left, upper, upper_right, left, right, lower_left, lower, lower_right, comparer, zero_even_bytes_mask);
    __m128i result_2 = applyFilter(upper_left_2, upper_2, upper_right_2, left_2, right_2, lower_left_2, lower_2, lower_right_2, comparer, zero_even_bytes_mask);

    //Shift the bits from addr + 1 in place
    result_2 = _mm_slli_epi16(result_2, 8);

    _mm_storeu_si128((__m128i *) (img_out + i), _mm_or_si128(result_2, result));
}

__attribute__((__always_inline__, __hot__)) inline
__m128i applyFilter(__m128i upper_left, __m128i upper, __m128i upper_right, __m128i left, __m128i right, __m128i lower_left, __m128i lower, __m128i lower_right, __m128i comparer, __m128i zero_even_bytes_mask) {
    /*
     * We avoid Byte overflows by using word sized integers. To do this, we load 16 Bytes from the current
     * address as well as from the current address + 1. This way we can use a bit mask to zero out every second byte
     * to convert 16 bytes in one xmm register to 8 words in two xmm registers. After the matrix operations are done accordingly,
     * we use cmpgt to set all values greater than 255 to -1. We then again zero out the higher order bits, and continue by
     * shifting the vector register containing the bytes from addr + 1 to the left. Then the vector registers are or-ed
     * and saved to img_out. This way we are able to achieve very accurate results whilst still benefiting from the ~x10
     * SIMD speedup.
    */

    upper_left = _mm_and_si128(upper_left, zero_even_bytes_mask);
    upper = _mm_and_si128(upper, zero_even_bytes_mask);
    upper_right = _mm_and_si128(upper_right, zero_even_bytes_mask);
    left = _mm_and_si128(left, zero_even_bytes_mask);
    right = _mm_and_si128(right, zero_even_bytes_mask);
    lower_left = _mm_and_si128(lower_left, zero_even_bytes_mask);
    lower = _mm_and_si128(lower, zero_even_bytes_mask);
    lower_right = _mm_and_si128(lower_right, zero_even_bytes_mask);

    // Multiplication with M_v for all color channels
    __m128i A_v = upper_left;
    A_v = _mm_sub_epi16(A_v, lower_left);

    A_v = _mm_add_epi16(A_v, upper);
    A_v = _mm_add_epi16(A_v, upper);
    A_v = _mm_sub_epi16(A_v, lower);
    A_v = _mm_sub_epi16(A_v, lower);

    A_v = _mm_add_epi16(A_v, upper_right);
    A_v = _mm_sub_epi16(A_v, lower_right);

    // Matrix multiplication with M_h for all color channels
    __m128i A_h = upper_left;
    A_h = _mm_sub_epi16(A_h, upper_right);

    A_h = _mm_add_epi16(A_h, left);
    A_h = _mm_add_epi16(A_h, left);
    A_h = _mm_sub_epi16(A_h, right);
    A_h = _mm_sub_epi16(A_h, right);

    A_h = _mm_add_epi16(A_h, lower_left);
    A_h = _mm_sub_epi16(A_h, lower_right);


    A_v = _mm_abs_epi16(A_v);
    A_h = _mm_abs_epi16(A_h);

    __m128i sum = _mm_add_epi16(A_v, A_h);

    //This handles all values > 255.
    __m128i result = _mm_and_si128(_mm_cmpgt_epi16(sum, comparer), zero_even_bytes_mask);

    //This handles all values < 255.
    return _mm_or_si128(result, _mm_and_si128(sum, zero_even_bytes_mask));
}
