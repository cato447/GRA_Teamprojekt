//
// Created by Cato on 21.06.23.
//

#include "image_similarity.h"

#include <stdio.h>

// gives ratio of differing pixels to total pixel count of image
// returns -1 if parameters are not valid
double compareImages(uint8_t *img1, size_t img1_size, uint8_t *img2, size_t img2_size) {
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
    for (size_t i = 0; i < img1_size; i += 3) {
        if (img1[i] != img2[i] || img1[i + 1] != img2[i + 1] || img1[i + 2] != img2[i + 2]) {
            // 1 pixel = 3 channels
            printf("Pixel %d: img 1 = (%d,%d,%d) | img 2 = (%d,%d,%d)\n", i/3, img1[i], img1[i + 1], img1[i + 2], img2[i], img2[i+1], img2[i+2]);
            differingPixels += 3;
        }
    }
    return 1.0 - ((double) differingPixels / img1_size);
}
