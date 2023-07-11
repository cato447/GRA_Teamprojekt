#include <stddef.h>
#include <pthread.h>
#include <stdio.h>

#include "basic_sobel.h"
#include "immintrin.h"
#include "simd_sobel.h"
#include "thread_sobel.h"

#define LINES_PER_THREAD 200

void thread_sobel(uint8_t *img_in, size_t width, size_t height, uint8_t *img_out) {
    if (width >= 16 && height > LINES_PER_THREAD) {
        size_t amountThreads = height / LINES_PER_THREAD - (height % LINES_PER_THREAD == 0 ? 1 : 0);
        pthread_t threads[amountThreads];

        //We need the first fromY to be 1 instead of 0, or we might access memory that we do not own.
        int ensureOffset = 1;

        for (int i = 0; i < amountThreads; ++i) {
            sobelIntervalArgs* args = (sobelIntervalArgs*)malloc(sizeof(sobelIntervalArgs));

            if (!args) {
                printf("Failed to allocate memory for thread %d. Aborting.", i);
                abort();
            }

            args -> img_in = img_in;
            args -> width = width;
            args -> fromY = i * LINES_PER_THREAD + ensureOffset;
            args -> toY = i * LINES_PER_THREAD + LINES_PER_THREAD;
            args -> img_out = img_out;

            int creationResult = pthread_create(&threads[i], PTHREAD_CREATE_JOINABLE, computeSobelForHeightInterval, (void*) args);

            if (creationResult != 0) {
                printf("Thread %d could not be created: %d. Aborting.\n", i, creationResult);
                abort();
            }

            ensureOffset = 0;
            //printf("\nCreated Thread %d: from = %zu, to = %zu, LPT = %d\n", i, args -> fromY, args -> toY, LINES_PER_THREAD);
        }

        //printf("\nCreated %zu Threads: h = %zu, LPT = %d\n", amountThreads, height, LINES_PER_THREAD);

        //Remaining image is calculated by normal simd_sobel with adjusted pointers and height
        size_t imgOffset = width * 3 * amountThreads * LINES_PER_THREAD - (amountThreads > 0 ? width * 3 : 0);

        simd_sobel(img_in + imgOffset, width, (height % LINES_PER_THREAD == 0 ? LINES_PER_THREAD : height % LINES_PER_THREAD) + (amountThreads > 0 ? 1 : 0), img_out + imgOffset);

        for (int i = 0; i < amountThreads; ++i) {
            pthread_join(threads[i], NULL);
        }
    } else {
        //If image is too small for the thread logic to work properly, fall back to normal simd
        simd_sobel(img_in, width, height, img_out);
    }
}

void *computeSobelForHeightInterval(void *args) {
    sobelIntervalArgs arguments = *(sobelIntervalArgs *) args;

    uint8_t *img_in = arguments.img_in;
    size_t width = arguments.width;
    size_t fromY = arguments.fromY;
    size_t toY = arguments.toY;
    uint8_t *img_out = arguments.img_out;

    __m128i comparer = _mm_set_epi16(COMP_255);
    __m128i zeroEvenBytesMask = _mm_set_epi16(ZERO_EVEN_BYTES_MASK);

    //printf("Calculating from %zu to %zu\n", fromY, toY);

    for (size_t i = width * fromY * 3 + 3; i < width * toY * 3; i += 16) {
        computeSIMDSobel(img_in, i, width, img_out, zeroEvenBytesMask, comparer);
    }
    free(args);
    return NULL;
}