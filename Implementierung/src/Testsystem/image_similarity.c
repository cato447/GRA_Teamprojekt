#include "image_similarity.h"

#include <stdio.h>

// gives ratio of differing pixels to total pixel count of image
// returns -1 if parameters are not valid
double compare_images(uint8_t *img_1, size_t img_1_size, uint8_t *img_2, size_t img_2_size) {
    if (img_1 == NULL) {
        fprintf(stderr, "Img1 is null\n");
        return -1;
    }
    if (img_2 == NULL) {
        fprintf(stderr, "Img2 is null\n");
        return -1;
    }
    if (img_1_size != img_2_size) {
        fprintf(stderr, "Images are not the same size\n");
        return -1;
    }
    size_t differing_pixels = 0;
    // every image has a black border after the sobel calculation
    // that's why we don't have to check those pixels
    for(size_t i = 0; i < img_1_size; i +=3){
        if(img_1[i] != img_2[i] ||img_1[i+1] != img_2[i+1] || img_1[i+2] != img_2[i+2]){
            differing_pixels += 3;
        }
    }
    return 1.0 - ((double) differing_pixels / img_1_size);
}

// gives ratio of differing pixels to total pixel count of image
// returns -1 if parameters are not valid
double
compare_images_graysc(uint8_t *img_1, size_t img_1_size, uint8_t *img_2, size_t img_2_size) {
    if (img_1 == NULL) {
        fprintf(stderr, "Img1 is null\n");
        return -1;
    }
    if (img_2 == NULL) {
        fprintf(stderr, "Img2 is null\n");
        return -1;
    }
    if (img_1_size != img_2_size) {
        fprintf(stderr, "Images are not the same size\n");
        return -1;
    }
    size_t differing_pixels = 0;
    // every image has a black border after the sobel calculation
    // that's why we don't have to check those pixels
    for(size_t i = 0; i < img_1_size; i++){
        if(img_1[i] != img_2[i]){
            differing_pixels++;
        }
    }
    return 1.0 - ((double) differing_pixels / img_1_size);
}
