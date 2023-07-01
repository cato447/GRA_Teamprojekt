#include "basic_sobel.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include "immintrin.h"
#include "simd_sobel.h"
#include <pthread.h>
#include "thread_sobel.h"
#include <stdio.h>

#define SHIFT_BY2 2
#define SHIFT_BY1 1
#define LINES_PER_THREAD 250

void thread_sobel(uint8_t* img_in, size_t width, size_t height, uint8_t* img_out) {
    if (width >= 16) {
        size_t amountThreads = height/LINES_PER_THREAD-1;
        pthread_t threads[amountThreads];
        for (int i = 0; i < amountThreads; ++i) {
            sobelIntervalArgs args;
            args.img_in = img_in;
            args.width = width;
            args.fromY = i * LINES_PER_THREAD;
            args.toY = i * LINES_PER_THREAD + LINES_PER_THREAD;
            args.img_out = img_out;
            pthread_create(&threads[i], NULL, computeSobelForHeightInterval, (void*) &args);
        }

        //printf("Created %zu Threads\n", amountThreads);

        sobelIntervalArgs args;
        args.img_in = img_in;
        args.width = width;
        args.fromY = height - height % LINES_PER_THREAD;
        args.toY = height;
        args.img_out = img_out;

        //printf("Self from %zu to %zu\n", args.fromY, args.toY);

        computeSobelForHeightInterval((void*) &args);
        for (int i = 0; i < amountThreads; ++i) {
            pthread_join(threads[i], NULL);
        }
    } else {
        sobel(img_in, width, height, img_out);
    }
}

void* computeSobelForHeightInterval(void* args) {
    //slightly different SIMD Implementation
    __m128i shrpi8Mask2 = _mm_loadu_si128((const __m128i*) SHIFT_BY2_MASK);
    __m128i shrpi8Mask1 = _mm_loadu_si128((const __m128i*) SHIFT_BY1_MASK);

    sobelIntervalArgs arguments = *(sobelIntervalArgs*) args;

    uint8_t* img_in = arguments.img_in;
    size_t width = arguments.width;
    size_t fromY = arguments.fromY;
    size_t toY = arguments.toY;
    uint8_t* img_out = arguments.img_out;

    for (size_t i = width * fromY * 3; i < width * toY * 3; i += 16) {
        __m128i A_v = _mm_setzero_si128();
        __m128i A_h = _mm_setzero_si128();

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
        upperLeft = _mm_and_si128(_mm_srli_epi16(upperLeft, SHIFT_BY2), shrpi8Mask2);
        upper = _mm_and_si128(_mm_srli_epi16(upper, SHIFT_BY1), shrpi8Mask1);
        upperRight = _mm_and_si128(_mm_srli_epi16(upperRight, SHIFT_BY2), shrpi8Mask2);
        left = _mm_and_si128(_mm_srli_epi16(left, SHIFT_BY1), shrpi8Mask1);
        right = _mm_and_si128(_mm_srli_epi16(right, SHIFT_BY1), shrpi8Mask1);
        lowerLeft = _mm_and_si128(_mm_srli_epi16(lowerLeft, SHIFT_BY2), shrpi8Mask2);
        lower = _mm_and_si128(_mm_srli_epi16(lower, SHIFT_BY1), shrpi8Mask1);
        lowerRight = _mm_and_si128(_mm_srli_epi16(lowerRight, SHIFT_BY2), shrpi8Mask2);

        // Multiplication with M_v for all color channels
        A_v = _mm_add_epi8(A_v, upperLeft);
        A_v = _mm_sub_epi8(A_v,  lowerLeft);

        A_v = _mm_add_epi8(A_v, upper);
        A_v = _mm_sub_epi8(A_v, lower);

        A_v = _mm_add_epi8(A_v, upperRight);
        A_v = _mm_sub_epi8(A_v, lowerRight);

        // Matrix multiplication with M_h for all color channels
        A_h = _mm_add_epi8(A_h, upperLeft);
        A_h = _mm_sub_epi8(A_h, upperRight);

        A_h = _mm_add_epi8(A_h, left);
        A_h = _mm_sub_epi8(A_h, right);

        A_h = _mm_add_epi8(A_h, lowerRight);
        A_h = _mm_sub_epi8(A_h, lowerLeft);

        A_v = _mm_abs_epi8(A_v);
        A_h = _mm_abs_epi8(A_h);

        // While it is still possible for this last sum to overflow, the chances are fairly low.
        _mm_storeu_si128((__m128i*) (img_out + i), _mm_add_epi8(A_v, A_h));
    }

    return NULL;
}
