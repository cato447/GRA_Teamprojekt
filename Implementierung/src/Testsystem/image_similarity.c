//
// Created by Cato on 21.06.23.
//

#include "image_similarity.h"

#include <stdio.h>

// gives ratio of differing pixels to total pixel count of image
// returns -1 if parameters are not valid
double
compareImages(uint8_t *img1, size_t img1_size, size_t img1_width, size_t img1_height, uint8_t *img2, size_t img2_size) {
    if (img1 == NULL) {
        fprintf(stderr, "Img1 is null\n");
        return -1;
    }
    if (img2 == NULL) {
        fprintf(stderr, "Img2 is null\n");
        return -1;
    }
    if (img1_size != img2_size) {
        fprintf(stderr, "Images are not the same size\n");
        return -1;
    }

    size_t differingPixels = 0;
    // every image has a black border after the sobel calculation
    // that's why we don't have to check those pixels
    for (size_t x = 1; x < img1_width - 1; x++) {
        for (size_t y = 1; y < img1_height - 1; y++) {
            if (img1[(y * img1_width + x) * 3] != img2[(y * img1_width + x) * 3] ||
                img1[(y * img1_width + x) * 3 + 1] != img2[(y * img1_width + x) * 3 + 1] ||
                img1[(y * img1_width + x) * 3 + 2] != img2[(y * img1_width + x) * 3 + 2]) {
                // 1 pixel = 3 channels
                printf("Pixel (%ld,%ld): sobel version 0 (img1) = (%d,%d,%d) | sobel to test (img2) = (%d,%d,%d)\n", x, y,
                       img1[(y * img1_width + x) * 3], img1[(y * img1_width + x) * 3 + 1],
                       img1[(y * img1_width + x) * 3 + 2], img2[(y * img1_width + x) * 3],
                       img2[(y * img1_width + x) * 3 + 1], img2[(y * img1_width + x) * 3 + 2]);
                differingPixels += 3;
            }
        }
    }
    return 1.0 - ((double) differingPixels / img1_size);
}
