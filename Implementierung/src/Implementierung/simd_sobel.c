#include <immintrin.h>
#include "basic_sobel.h"
#include "simd_sobel.h"

void simd_sobel(uint8_t* img_in, size_t width, size_t height, uint8_t* img_out) {
    if (width >= 16 && height >= 3) {
        __m128i zeroEvenBytesMask = _mm_set_epi16(ZERO_EVEN_BYTES_MASK);
        __m128i comparer = _mm_set_epi16(COMP_255);

        size_t i;

        for (i = width * 3 + 3; i < width * (height-1) * 3 - 3 - 16; i += 16) {
            computeSIMDSobel(img_in, i, width, img_out, zeroEvenBytesMask, comparer);
        }

        for ( ; i < width * (height-1) * 3 - 3; ++i) {
            uint8_t upperLeft = *(img_in + i - width * 3 - 3);
            uint8_t upper = *(img_in + i - width * 3);
            uint8_t upperRight = *(img_in + i - width * 3 + 3);
            uint8_t left = *(img_in + i - 3);
            uint8_t right = *(img_in + i + 3);
            uint8_t lowerLeft = *(img_in + i + width * 3 - 3);
            uint8_t lower = *(img_in + i + width * 3);
            uint8_t lowerRight = *(img_in + i + width * 3 + 3);

            int32_t A_v = upperLeft - lowerLeft + 2 * upper - 2 * lower + upperRight - lowerRight;
            int32_t A_h = upperLeft - upperRight + 2 * left - 2 * right + lowerLeft - lowerRight;

            *(img_out + i) = abs(A_v) + abs(A_h) > 255 ? 255 : abs(A_v) + abs(A_h);
        }
    } else {
        sobel(img_in, width, height, img_out);
    }
}

__attribute__((__always_inline__, __hot__)) inline
void computeSIMDSobel(uint8_t* img_in, size_t i, size_t width, uint8_t* img_out, __m128i zeroEvenBytesMask, __m128i comparer) {
    __m128i upperLeft = _mm_loadu_si128((const __m128i*) (img_in + i - width * 3 - 3));
    __m128i upper = _mm_loadu_si128((const __m128i*) (img_in + i - width * 3));
    __m128i upperRight = _mm_loadu_si128((const __m128i*) (img_in + i - width * 3 + 3));
    __m128i left = _mm_loadu_si128((const __m128i*) (img_in + i - 3));
    __m128i right = _mm_loadu_si128((const __m128i*) (img_in + i + 3));
    __m128i lowerLeft = _mm_loadu_si128((const __m128i*) (img_in + i + width * 3 - 3));
    __m128i lower = _mm_loadu_si128((const __m128i*) (img_in + i + width * 3));
    __m128i lowerRight = _mm_loadu_si128((const __m128i*) (img_in + i + width * 3 + 3));

    __m128i upperLeft2 = _mm_loadu_si128((const __m128i*) (img_in + i - width * 3 - 3 + 1));
    __m128i upper2 = _mm_loadu_si128((const __m128i*) (img_in + i - width * 3 + 1));
    __m128i upperRight2 = _mm_loadu_si128((const __m128i*) (img_in + i - width * 3 + 3 + 1));
    __m128i left2 = _mm_loadu_si128((const __m128i*) (img_in + i - 3 + 1));
    __m128i right2 = _mm_loadu_si128((const __m128i*) (img_in + i + 3 + 1));
    __m128i lowerLeft2 = _mm_loadu_si128((const __m128i*) (img_in + i + width * 3 - 3 + 1));
    __m128i lower2 = _mm_loadu_si128((const __m128i*) (img_in + i + width * 3 + 1));
    __m128i lowerRight2 = _mm_loadu_si128((const __m128i*) (img_in + i + width * 3 + 3 + 1));


    /*
     * We avoid Byte overflows by using word sized integers. To do this, we load 16 Bytes from the current
     * address as well as from the current address + 1. This way we can use a bit mask to zero out every second byte
     * to convert 16 bytes in one xmm register to 8 words in two xmm registers. After the matrix operations are done accordingly,
     * we use cmpgt to set all values greater than 255 to -1. We then again zero out the higher order bits, and continue by
     * shifting the vector register containing the bytes from addr + 1 to the left. Then the vector registers are or-ed
     * and saved to img_out. This way we are able to achieve very accurate results whilst still benefiting from the ~x10
     * SIMD speedup.
    */

    upperLeft = _mm_and_si128(upperLeft,  zeroEvenBytesMask);
    upper = _mm_and_si128(upper,  zeroEvenBytesMask);
    upperRight = _mm_and_si128(upperRight,  zeroEvenBytesMask);
    left = _mm_and_si128(left,  zeroEvenBytesMask);
    right = _mm_and_si128(right,  zeroEvenBytesMask);
    lowerLeft = _mm_and_si128(lowerLeft,  zeroEvenBytesMask);
    lower = _mm_and_si128(lower,  zeroEvenBytesMask);
    lowerRight = _mm_and_si128(lowerRight, zeroEvenBytesMask);

    upperLeft2 = _mm_and_si128(upperLeft2,  zeroEvenBytesMask);
    upper2 = _mm_and_si128(upper2,  zeroEvenBytesMask);
    upperRight2 = _mm_and_si128(upperRight2,  zeroEvenBytesMask);
    left2 = _mm_and_si128(left2,  zeroEvenBytesMask);
    right2 = _mm_and_si128(right2,  zeroEvenBytesMask);
    lowerLeft2 = _mm_and_si128(lowerLeft2,  zeroEvenBytesMask);
    lower2 = _mm_and_si128(lower2,  zeroEvenBytesMask);
    lowerRight2 = _mm_and_si128(lowerRight2, zeroEvenBytesMask);


    // Multiplication with M_v for all color channels
    __m128i A_v = upperLeft;
    A_v = _mm_sub_epi16(A_v,  lowerLeft);

    A_v = _mm_add_epi16(A_v, upper);
    A_v = _mm_add_epi16(A_v, upper);
    A_v = _mm_sub_epi16(A_v, lower);
    A_v = _mm_sub_epi16(A_v, lower);

    A_v = _mm_add_epi16(A_v, upperRight);
    A_v = _mm_sub_epi16(A_v, lowerRight);


    __m128i A_v2 = upperLeft2;
    A_v2 = _mm_sub_epi16(A_v2,  lowerLeft2);

    A_v2 = _mm_add_epi16(A_v2, upper2);
    A_v2 = _mm_add_epi16(A_v2, upper2);
    A_v2 = _mm_sub_epi16(A_v2, lower2);
    A_v2 = _mm_sub_epi16(A_v2, lower2);

    A_v2 = _mm_add_epi16(A_v2, upperRight2);
    A_v2 = _mm_sub_epi16(A_v2, lowerRight2);

    // Matrix multiplication with M_h for all color channels
    __m128i A_h = upperLeft;
    A_h = _mm_sub_epi16(A_h, upperRight);

    A_h = _mm_add_epi16(A_h, left);
    A_h = _mm_add_epi16(A_h, left);
    A_h = _mm_sub_epi16(A_h, right);
    A_h = _mm_sub_epi16(A_h, right);

    A_h = _mm_add_epi16(A_h, lowerLeft);
    A_h = _mm_sub_epi16(A_h, lowerRight);


    __m128i A_h2 = upperLeft2;
    A_h2 = _mm_sub_epi16(A_h2, upperRight2);

    A_h2 = _mm_add_epi16(A_h2, left2);
    A_h2 = _mm_add_epi16(A_h2, left2);
    A_h2 = _mm_sub_epi16(A_h2, right2);
    A_h2 = _mm_sub_epi16(A_h2, right2);

    A_h2 = _mm_add_epi16(A_h2, lowerLeft2);
    A_h2 = _mm_sub_epi16(A_h2, lowerRight2);


    A_v2 = _mm_abs_epi16(A_v2);
    A_h2 = _mm_abs_epi16(A_h2);
    A_v =  _mm_abs_epi16(A_v);
    A_h =  _mm_abs_epi16(A_h);

    __m128i sum = _mm_add_epi16(A_v, A_h);
    __m128i sum2 =_mm_add_epi16(A_v2, A_h2);

    //This handles all values > 255.
    __m128i result = _mm_and_si128(_mm_cmpgt_epi16(sum, comparer), zeroEvenBytesMask);
    __m128i result2 = _mm_and_si128(_mm_cmpgt_epi16(sum2, comparer), zeroEvenBytesMask);

    //This handles all values < 255.
    result = _mm_or_si128(result, _mm_and_si128(sum, zeroEvenBytesMask));
    result2 = _mm_or_si128(result2, _mm_and_si128(sum2, zeroEvenBytesMask));

    //Shift the bits from addr + 1 in place
    result2 = _mm_slli_epi16(result2, 8);

    _mm_storeu_si128((__m128i*) (img_out + i), _mm_or_si128(result2, result));
}