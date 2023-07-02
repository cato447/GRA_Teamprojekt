#include "basic_sobel.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include "immintrin.h"
#include "simd_sobel.h"

const uint8_t ZERO_EVEN_BYTES_MASK[] =
        {
            0xFF, 0x0, 0xFF, 0x0,0xFF,
            0x0, 0xFF, 0x0,0xFF, 0x0,
            0xFF, 0x0,0xFF, 0x0, 0xFF, 0x0
        };

const uint16_t COMP_255[] =
        {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
        };

void simd_sobel(uint8_t* img_in, size_t width, size_t height, uint8_t* img_out) {
    if (width >= 16) {
        __m128i zeroEvenBytesMask = _mm_loadu_si128((const __m128i*) ZERO_EVEN_BYTES_MASK);
        for (size_t i = width * 3 + 3; i < width * (height-1) * 3 - 3; i += 16) {
            __m128i A_v = _mm_setzero_si128();
            __m128i A_h = _mm_setzero_si128();

            __m128i A_v2 = _mm_setzero_si128();
            __m128i A_h2 = _mm_setzero_si128();

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
             * We avoid Byte overflows by using word sized integers. To do this, we load 16 Byte from the current
             * address as well as from the current address + 1. This way we can use a bit mask to zero out every second byte
             * to convert 16 bytes in one xmm register to 8 words in two xmm registers.
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
            A_v = _mm_add_epi16(A_v, upperLeft);
            A_v = _mm_sub_epi16(A_v,  lowerLeft);

            A_v = _mm_add_epi16(A_v, upper);
            A_v = _mm_add_epi16(A_v, upper);
            A_v = _mm_sub_epi16(A_v, lower);
            A_v = _mm_sub_epi16(A_v, lower);

            A_v = _mm_add_epi16(A_v, upperRight);
            A_v = _mm_sub_epi16(A_v, lowerRight);


            A_v2 = _mm_add_epi16(A_v2, upperLeft2);
            A_v2 = _mm_sub_epi16(A_v2,  lowerLeft2);

            A_v2 = _mm_add_epi16(A_v2, upper2);
            A_v2 = _mm_add_epi16(A_v2, upper2);
            A_v2 = _mm_sub_epi16(A_v2, lower2);
            A_v2 = _mm_sub_epi16(A_v2, lower2);

            A_v2 = _mm_add_epi16(A_v2, upperRight2);
            A_v2 = _mm_sub_epi16(A_v2, lowerRight2);

            // Matrix multiplication with M_h for all color channels
            A_h = _mm_add_epi16(A_h, upperLeft);
            A_h = _mm_sub_epi16(A_h, upperRight);

            A_h = _mm_add_epi16(A_h, left);
            A_h = _mm_add_epi16(A_h, left);
            A_h = _mm_sub_epi16(A_h, right);
            A_h = _mm_sub_epi16(A_h, right);

            A_h = _mm_add_epi16(A_h, lowerLeft);
            A_h = _mm_sub_epi16(A_h, lowerRight);


            A_h2 = _mm_add_epi16(A_h2, upperLeft2);
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
            
            __m128i result = _mm_and_si128(_mm_cmpgt_epi16(sum, _mm_loadu_si128((const __m128i*) COMP_255)), zeroEvenBytesMask);
            __m128i result2 = _mm_and_si128(_mm_cmpgt_epi16(sum2, _mm_loadu_si128((const __m128i*) COMP_255)), zeroEvenBytesMask);

            result = _mm_or_si128(result, sum);
            result2 = _mm_or_si128(result2, sum2);

            result2 = _mm_slli_epi16(result2, 8);

            _mm_storeu_si128((__m128i*) (img_out + i), _mm_or_si128(result2, result));
        }
    } else {
        sobel(img_in, width, height, img_out);
    }
}