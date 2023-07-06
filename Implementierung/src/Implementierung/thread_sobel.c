#include "basic_sobel.h"
#include <stddef.h>
#include "immintrin.h"
#include "simd_sobel.h"
#include <pthread.h>
#include "thread_sobel.h"
#include <stdio.h>
#define LINES_PER_THREAD 200

void thread_sobel(uint8_t *img_in, size_t width, size_t height, uint8_t *img_out) {
    if (width >= 16 && height >= 3) {
        size_t amountThreads = height / LINES_PER_THREAD - (height % LINES_PER_THREAD == 0);
        pthread_t threads[amountThreads];

        //We need the first fromY to be 1 instead of 0, or we might access memory that we do not own.
        int ensureOffset = 1;

        printf("amount: %zu", amountThreads);

        for (int i = 0; i < amountThreads; ++i) {
            sobelIntervalArgs args;
            args.img_in = img_in;
            args.width = width;
            args.fromY = i * LINES_PER_THREAD + ensureOffset;
            args.toY = i * LINES_PER_THREAD + LINES_PER_THREAD;
            args.img_out = img_out;

            int creationResult;
            creationResult = pthread_create(&threads[i], PTHREAD_CREATE_JOINABLE, computeSobelForHeightInterval, (void*) &args);

            if (creationResult != 0) {
                printf("Thread %d raised an error: %d\n", i, creationResult);
            }

            ensureOffset = 0;
            printf("\nCreated Thread %d: from = %zu, to = %zu, LPT = %d\n", i, args.fromY, args.toY, LINES_PER_THREAD);
        }

        printf("\nCreated %zu Threads: h = %zu, LPT = %d\n", amountThreads, height, LINES_PER_THREAD);


        //TODO: Letzten Aufruf in eigene Funktion verlegen um Invalid read analog zu SIMD zu fixen

        sobelIntervalArgs args;
        args.img_in = img_in;
        args.width = width;
        args.fromY = height - height % LINES_PER_THREAD;
        args.toY = height - 1;
        args.img_out = img_out;

        printf("Self from %zu to %zu\n", args.fromY, args.toY);

        computeSobelForHeightInterval((void*) &args);

        void* result;

        for (int i = 0; i < amountThreads; ++i) {
            pthread_join(threads[i], &result);
            if (result != NULL) {
                printf("Thread %d returned with value\n", *((int*)result));
                result = NULL;
            }
            printf("Joined Thread %d\n", i);
        }
    } else {
        sobel(img_in, width, height, img_out);
    }
}

void *computeSobelForHeightInterval(void *args) {
    sobelIntervalArgs arguments = *(sobelIntervalArgs *) args;

    uint8_t *img_in = arguments.img_in;
    size_t width = arguments.width;
    size_t fromY = arguments.fromY;
    size_t toY = arguments.toY;
    uint8_t *img_out = arguments.img_out;

    __m128i comparer = _mm_loadu_si128((const __m128i*) COMP_255);
    __m128i zeroEvenBytesMask = _mm_loadu_si128((const __m128i *) ZERO_EVEN_BYTES_MASK);
    for (size_t i = width * fromY * 3 + 3; i < width * toY * 3; i += 16) {
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
    pthread_t thread_id = pthread_self();
    printf("Thread %llu finished work.\n", thread_id);
    return NULL;
}
