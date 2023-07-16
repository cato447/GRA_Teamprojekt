#include "sobel_basic.h"

#include <stdlib.h>

static const int8_t M_v[3][3] = { { 1, 0, -1 },
                           { 2, 0, -2 },
                           { 1, 0, -1 } };

static const int8_t M_h[3][3] = { { 1, 2, 1 },
                           { 0, 0, 0 },
                           { -1, -2, -1 } };

void sobel(const uint8_t *img_in, size_t width, size_t height, uint8_t *img_out) {
    for (size_t x = 1; x < width - 1; ++x) {
        for (size_t y = 1; y < height - 1; ++y) {
            int32_t A_v_r = 0;
            int32_t A_h_r = 0;

            int32_t A_v_g = 0;
            int32_t A_h_g = 0;

            int32_t A_v_b = 0;
            int32_t A_h_b = 0;

            for (int8_t i = -1; i <= 1; i++) {
                for (int8_t j = -1; j <= 1; j++) {
                    A_v_r += (int32_t)(M_v[1 + i][1 + j] * (color_of_pixel(img_in, width, x + i, y + j, RED)));
                    A_h_r += (int32_t)(M_h[1 + i][1 + j] * (color_of_pixel(img_in, width, x + i, y + j, RED)));

                    A_v_g += (int32_t)(M_v[1 + i][1 + j] * (color_of_pixel(img_in, width, x + i, y + j, GREEN)));
                    A_h_g += (int32_t)(M_h[1 + i][1 + j] * (color_of_pixel(img_in, width, x + i, y + j, GREEN)));

                    A_v_b += (int32_t)(M_v[1 + i][1 + j] * (color_of_pixel(img_in, width, x + i, y + j, BLUE)));
                    A_h_b += (int32_t)(M_h[1 + i][1 + j] * (color_of_pixel(img_in, width, x + i, y + j, BLUE)));
                }
            }

            int32_t G_r = abs(A_v_r) + abs(A_h_r);
            int32_t G_g = abs(A_v_g) + abs(A_h_g);
            int32_t G_b = abs(A_v_b) + abs(A_h_b);

            //Cap value at 255
            set_pixel_at(img_out, width, x, y, G_r > 255 ? 255 : G_r, G_g  > 255 ? 255 : G_g, G_b  > 255 ? 255 : G_b);
        }
    }
}

void sobel_graysc(const uint8_t *img_in, size_t width, size_t height, uint8_t *img_out) {
    for (size_t x = 1; x < width - 1; ++x) {
        for (size_t y = 1; y < height - 1; ++y) {
            int32_t A_v = 0;
            int32_t A_h = 0;

            for (int8_t i = -1; i <= 1; i++) {
                for (int8_t j = -1; j <= 1; j++) {
                    A_v += (int32_t)(M_v[1 + i][1 + j] * img_in[(y + j) * width + x + i]);
                    A_h += (int32_t)(M_h[1 + i][1 + j] * img_in[(y + j) * width + x + i]);
                }
            }

            int32_t G = abs(A_v) + abs(A_h);

            //Cap value at 255
            img_out[y * width + x] = G > 255 ? 255 : G;
        }
    }
}

uint8_t color_of_pixel(const uint8_t *img, size_t width, size_t x, size_t y, enum Color color) {
    // Pixel are 24 bit wide to compensate this we multiply x and y by 3
    return *(img + (width * 3) * y + x * 3 + color);
}

void set_pixel_at(uint8_t *img, size_t width, size_t x, size_t y, uint8_t red, uint8_t green, uint8_t blue) {
    // Pixel are 24 bit wide to compensate this we multiply x and y by 3
    *(img + (width * 3) * y + x * 3 + BLUE) = blue;
    *(img + (width * 3) * y + x * 3 + GREEN) = green;
    *(img + (width * 3) * y + x * 3 + RED) = red;
}
