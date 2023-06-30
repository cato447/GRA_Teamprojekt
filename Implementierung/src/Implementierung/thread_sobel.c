#include "basic_sobel.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include "immintrin.h"
#include "simd_sobel.h"
#include "thread_sobel.h"

#define SHIFT_BY 2

const uint8_t cleanUp16BitRShifts[] = { 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F };

void thread_sobel(uint8_t* img_in, size_t width, size_t height, uint8_t* img_out) {
    if (width >= 16) {
        __m128i shrpi8Mask2 = _mm_loadu_si128((const __m128i*) cleanUp16BitRShifts);

        for (size_t i = width * 3 + 3; i < width * (height-1) * 3 - 3; i += 16) {
            __m128i A_v = _mm_loadu_si128((const __m128i*) (img_in + i));
            __m128i A_h = _mm_loadu_si128((const __m128i*) (img_in + i));

            __m128i upperLeft = _mm_loadu_si128((const __m128i*) (img_in + i - width * 3 - 3));
            __m128i upper = _mm_loadu_si128((const __m128i*) (img_in + i - width * 3));
            __m128i upperRight = _mm_loadu_si128((const __m128i*) (img_in + i - width * 3 + 3));
            __m128i left = _mm_loadu_si128((const __m128i*) (img_in + i - 3));
            __m128i right = _mm_loadu_si128((const __m128i*) (img_in + i + 3));
            __m128i lowerLeft = _mm_loadu_si128((const __m128i*) (img_in + i + width * 3 - 3));
            __m128i lower = _mm_loadu_si128((const __m128i*) (img_in + i + width * 3));
            __m128i lowerRight = _mm_loadu_si128((const __m128i*) (img_in + i + width * 3 + 3));

            // Shifting 16 bit ints right by 2 and zeroing the two highest-order bits to simulate an 8 bit int shift.
            // We need to do this in order to prevent overflows that would otherwise be practically impossible to detect.
            // The downside of this is that the resulting image will be darker.
            A_v = _mm_and_si128(_mm_srli_epi16(A_v, SHIFT_BY), shrpi8Mask2);
            A_h = _mm_and_si128(_mm_srli_epi16(A_h, SHIFT_BY), shrpi8Mask2);

            upperLeft = _mm_and_si128(_mm_srli_epi16(upperLeft, SHIFT_BY), shrpi8Mask2);
            upper = _mm_and_si128(_mm_srli_epi16(upper, SHIFT_BY), shrpi8Mask2);
            upperRight = _mm_and_si128(_mm_srli_epi16(upperRight, SHIFT_BY), shrpi8Mask2);
            left = _mm_and_si128(_mm_srli_epi16(left, SHIFT_BY), shrpi8Mask2);
            right = _mm_and_si128(_mm_srli_epi16(right, SHIFT_BY), shrpi8Mask2);
            lowerLeft = _mm_and_si128(_mm_srli_epi16(lowerLeft, SHIFT_BY), shrpi8Mask2);
            lower = _mm_and_si128(_mm_srli_epi16(lower, SHIFT_BY), shrpi8Mask2);
            lowerRight = _mm_and_si128(_mm_srli_epi16(lowerRight, SHIFT_BY), shrpi8Mask2);


            // Multiplication with M_v for all color channels
            A_v = _mm_add_epi8(A_v, upperLeft);
            A_v = _mm_sub_epi8(A_v,  upperRight);

            A_v = _mm_add_epi8(A_v, left);
            A_v = _mm_add_epi8(A_v, left);  //TODO: Test performance of double addition/subtraction vs. single addition/subtraction and left shift / only one right shift
            A_v = _mm_sub_epi8(A_v, right);
            A_v = _mm_sub_epi8(A_v, right);

            A_v = _mm_add_epi8(A_v, lowerRight);
            A_v = _mm_sub_epi8(A_v, lowerLeft);

            // Matrix multiplication with M_h for all color channels
            A_h = _mm_add_epi8(A_h, upperLeft);
            A_h = _mm_sub_epi8(A_h, lowerLeft);

            A_h = _mm_add_epi8(A_h, upper);
            A_h = _mm_add_epi8(A_h, upper);  //TODO: Test performance of double addition/subtraction vs. single addition/subtraction and left shift / only one right shift
            A_h = _mm_sub_epi8(A_h, lower);
            A_h = _mm_sub_epi8(A_h, lower);

            A_h = _mm_add_epi8(A_h, upperRight);
            A_h = _mm_sub_epi8(A_h, lowerLeft);

            A_v = _mm_abs_epi8(A_v);
            A_h = _mm_abs_epi8(A_h);

            // While it is still possible for this last sum to overflow, the chances are fairly low.
            _mm_store_si128((__m128i*) (img_out + i), (_mm_add_epi8(A_v, A_h)));
        }
    } else {
        basic_sobel(img_in, width, height, img_out);
    }
}