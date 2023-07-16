#include "sobel_threaded.h"

#include <stddef.h>
#include <pthread.h>
#include <stdio.h>
#include <immintrin.h>
#include <sys/sysinfo.h>
#include <stdbool.h>

#include "sobel_basic.h"
#include "sobel_simd.h"

typedef struct sobel_interval_args {
    const uint8_t *img_in;
    size_t width;
    size_t from_y;
    size_t to_y;
    uint8_t *img_out;
    bool graysc;
} sobel_interval_args;

static void *compute_sobel_for_height_interval(void *args) {
    sobel_interval_args arguments = *(sobel_interval_args *) args;

    const uint8_t *img_in = arguments.img_in;
    size_t width = arguments.width;
    size_t from_y = arguments.from_y;
    size_t to_y = arguments.to_y;
    uint8_t *img_out = arguments.img_out;

    __m128i comparer = _mm_set_epi16(COMP_255);
    __m128i zero_even_bytes_mask = _mm_set_epi16(ZERO_EVEN_BYTES_MASK);

    if (!arguments.graysc) {
        for (size_t i = width * from_y * 3 + 3; i < width * to_y * 3; i += 16) {
            compute_sobel_simd(img_in, i, width, img_out, zero_even_bytes_mask, comparer);
        }
        for (size_t y = from_y; y <= to_y; y++) {
            set_pixel_at(img_out, width, 0, y, 0, 0, 0);
            set_pixel_at(img_out, width, width-1, y, 0, 0, 0);
        }
    } else {
        for (size_t i = width * from_y + 1; i < width * to_y; i += 16) {
            compute_sobel_simd_graysc(img_in, i, width, img_out, zero_even_bytes_mask, comparer);
        }
        for (size_t y = from_y; y <= to_y; y++) {
            img_out[y * width] = 0;
            img_out[y * width + width - 1] = 0;
        }
    }

    return NULL;
}

#define LINES_PER_THREAD_NO_NPROC 300

void sobel_threaded(const uint8_t *img_in, size_t width, size_t height, uint8_t *img_out) {
    size_t lines_per_thread = LINES_PER_THREAD_NO_NPROC;
    if (__linux__) {
        size_t hw_threads = get_nprocs();
        if (hw_threads > 1) {
            lines_per_thread = height / (hw_threads - 1);
        }
    }

    if (width * 3 * height >= 16 * 3 + 3 + 3 && height >= lines_per_thread) {
        size_t amount_threads = height / lines_per_thread - (height % lines_per_thread == 0 ? 1 : 0);
        pthread_t threads[amount_threads];
        sobel_interval_args args[amount_threads];

        //We need the first from_y to be 1 instead of 0, or we might access memory that we do not own.
        int ensure_offset = 1;

        for (size_t i = 0; i < amount_threads; ++i) {
            args[i].img_in = img_in;
            args[i].width = width;
            args[i].from_y = i * lines_per_thread + ensure_offset;
            args[i].to_y = i * lines_per_thread + lines_per_thread;
            args[i].img_out = img_out;
            args[i].graysc = false;

            int creation_result = pthread_create(&threads[i], PTHREAD_CREATE_JOINABLE, compute_sobel_for_height_interval, args + i);

            if (creation_result != 0) {
                printf("Thread %lu could not be created: %d. Aborting.\n", i, creation_result);
                abort();
            }

            ensure_offset = 0;
        }

        //Remaining image is calculated by normal simd_sobel with adjusted pointers and height
        size_t img_offset = width * 3 * amount_threads * lines_per_thread - (amount_threads > 0 ? width * 3 : 0);

        sobel_simd(img_in + img_offset, width, (height % lines_per_thread == 0 ? lines_per_thread : height % lines_per_thread) + (amount_threads > 0 ? 1 : 0), img_out + img_offset);

        for (size_t i = 0; i < amount_threads; ++i) {
            pthread_join(threads[i], NULL);
        }
    } else {
        //If image is too small for the thread logic to work properly, fall back to normal simd
        sobel_simd(img_in, width, height, img_out);
    }
}

void sobel_threaded_graysc(const uint8_t *img_in, size_t width, size_t height, uint8_t *img_out) {
    size_t lines_per_thread = 300;
    if (__linux__) {
        size_t hw_threads = get_nprocs();
        if (hw_threads > 1) {
            lines_per_thread = height / (hw_threads-1);
        }
    }

    if (width * height >= 16 + 1 + 1 && height >= lines_per_thread) {
        size_t amount_threads = height / lines_per_thread - (height % lines_per_thread == 0 ? 1 : 0);
        pthread_t threads[amount_threads];
        sobel_interval_args args[amount_threads];

        //We need the first from_y to be 1 instead of 0, or we might access memory that we do not own.
        int ensure_offset = 1;

        for (size_t i = 0; i < amount_threads; ++i) {
            args[i].img_in = img_in;
            args[i].width = width;
            args[i].from_y = i * lines_per_thread + ensure_offset;
            args[i].to_y = i * lines_per_thread + lines_per_thread;
            args[i].img_out = img_out;
            args[i].graysc = true;

            int creation_result = pthread_create(&threads[i], PTHREAD_CREATE_JOINABLE, compute_sobel_for_height_interval, args + i);

            if (creation_result != 0) {
                printf("Thread %lu could not be created: %d. Aborting.\n", i, creation_result);
                abort();
            }

            ensure_offset = 0;
        }

        //Remaining image is calculated by normal sobel_simd with adjusted pointers and height
        size_t img_offset = width * amount_threads * lines_per_thread - (amount_threads > 0 ? width : 0);

        sobel_simd_graysc(img_in + img_offset, width, (height % lines_per_thread == 0 ? lines_per_thread : height % lines_per_thread) + (amount_threads > 0 ? 1 : 0), img_out + img_offset);

        for (size_t i = 0; i < amount_threads; ++i) {
            pthread_join(threads[i], NULL);
        }
    } else {
        //If image is too small for the thread logic to work properly, fall back to normal simd
        sobel_simd(img_in, width, height, img_out);
    }
}
