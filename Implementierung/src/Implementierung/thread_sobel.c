#include "thread_sobel.h"

#include <stddef.h>
#include <pthread.h>
#include <stdio.h>
#include <immintrin.h>
#include <sys/sysinfo.h>
#include <stdbool.h>

#include "basic_sobel.h"
#include "simd_sobel.h"

#if __linux__ == 1
    #define THREADCALC size_t hwThreads = get_nprocs(); \
        size_t LINES_PER_THREAD = 300; \
        if (hwThreads > 1) { \
            LINES_PER_THREAD = height / (hwThreads-1); \
        }
#else
    #define THREADCALC size_t LINES_PER_THREAD = 300;
#endif

typedef struct sobelIntervalArgs {
    uint8_t* img_in;
    size_t width;
    size_t fromY;
    size_t toY;
    uint8_t* img_out;
    bool graysc;
} sobelIntervalArgs;

void *computeSobelForHeightInterval(void *args) {
    sobelIntervalArgs arguments = *(sobelIntervalArgs *) args;

    uint8_t *img_in = arguments.img_in;
    size_t width = arguments.width;
    size_t fromY = arguments.fromY;
    size_t toY = arguments.toY;
    uint8_t *img_out = arguments.img_out;

    __m128i comparer = _mm_set_epi16(COMP_255);
    __m128i zeroEvenBytesMask = _mm_set_epi16(ZERO_EVEN_BYTES_MASK);

    if (!arguments.graysc) {
        for (size_t i = width * fromY * 3 + 3; i < width * toY * 3; i += 16) {
            computeSIMDSobel(img_in, i, width, img_out, zeroEvenBytesMask, comparer);
        }
        for (size_t y = fromY; y < toY; y++) {
            setPixelAt(img_out, width, 0, y, 0, 0, 0);
            setPixelAt(img_out, width, width-1, y, 0, 0, 0);
        }
    } else {
        for (size_t i = width * fromY + 1; i < width * toY; i += 16) {
            computeSIMDSobel_graysc(img_in, i, width, img_out, zeroEvenBytesMask, comparer);
        }
        for (size_t y = fromY; y < toY; y++) {
            img_out[y * width] = 0;
            img_out[y * width + width - 1] = 0;
        }
    }

    return NULL;
}

void thread_sobel(uint8_t *img_in, size_t width, size_t height, uint8_t *img_out) {
    THREADCALC

    if (width * 3 * height >= 16 * 3 + 3 + 3 && height >= LINES_PER_THREAD) {
        size_t amountThreads = height / LINES_PER_THREAD - (height % LINES_PER_THREAD == 0 ? 1 : 0);
        pthread_t threads[amountThreads];
        sobelIntervalArgs args[amountThreads];

        //We need the first fromY to be 1 instead of 0, or we might access memory that we do not own.
        int ensureOffset = 1;

        for (size_t i = 0; i < amountThreads; ++i) {
            args[i].img_in = img_in;
            args[i].width = width;
            args[i].fromY = i * LINES_PER_THREAD + ensureOffset;
            args[i].toY = i * LINES_PER_THREAD + LINES_PER_THREAD;
            args[i].img_out = img_out;
            args[i].graysc = false;

            int creationResult = pthread_create(&threads[i], PTHREAD_CREATE_JOINABLE, computeSobelForHeightInterval, args + i);

            if (creationResult != 0) {
                printf("Thread %lu could not be created: %d. Aborting.\n", i, creationResult);
                abort();
            }

            ensureOffset = 0;
        }

        //Remaining image is calculated by normal simd_sobel with adjusted pointers and height
        size_t imgOffset = width * 3 * amountThreads * LINES_PER_THREAD - (amountThreads > 0 ? width * 3 : 0);

        simd_sobel(img_in + imgOffset, width, (height % LINES_PER_THREAD == 0 ? LINES_PER_THREAD : height % LINES_PER_THREAD) + (amountThreads > 0 ? 1 : 0), img_out + imgOffset);

        for (size_t i = 0; i < amountThreads; ++i) {
            pthread_join(threads[i], NULL);
        }
    } else {
        //If image is too small for the thread logic to work properly, fall back to normal simd
        simd_sobel(img_in, width, height, img_out);
    }
}

void thread_sobel_graysc(uint8_t *img_in, size_t width, size_t height, uint8_t *img_out) {
    THREADCALC

    if (width * height >= 16 + 1 + 1 && height >= LINES_PER_THREAD) {
        size_t amountThreads = height / LINES_PER_THREAD - (height % LINES_PER_THREAD == 0 ? 1 : 0);
        pthread_t threads[amountThreads];
        sobelIntervalArgs args[amountThreads];

        //We need the first fromY to be 1 instead of 0, or we might access memory that we do not own.
        int ensureOffset = 1;

        for (size_t i = 0; i < amountThreads; ++i) {
            args[i].img_in = img_in;
            args[i].width = width;
            args[i].fromY = i * LINES_PER_THREAD + ensureOffset;
            args[i].toY = i * LINES_PER_THREAD + LINES_PER_THREAD;
            args[i].img_out = img_out;
            args[i].graysc = true;

            int creationResult = pthread_create(&threads[i], PTHREAD_CREATE_JOINABLE, computeSobelForHeightInterval, args + i);

            if (creationResult != 0) {
                printf("Thread %lu could not be created: %d. Aborting.\n", i, creationResult);
                abort();
            }

            ensureOffset = 0;
        }

        //Remaining image is calculated by normal simd_sobel with adjusted pointers and height
        size_t imgOffset = width * amountThreads * LINES_PER_THREAD - (amountThreads > 0 ? width : 0);

        simd_sobel_graysc(img_in + imgOffset, width, (height % LINES_PER_THREAD == 0 ? LINES_PER_THREAD : height % LINES_PER_THREAD) + (amountThreads > 0 ? 1 : 0), img_out + imgOffset);

        for (size_t i = 0; i < amountThreads; ++i) {
            pthread_join(threads[i], NULL);
        }
    } else {
        //If image is too small for the thread logic to work properly, fall back to normal simd
        simd_sobel(img_in, width, height, img_out);
    }
}