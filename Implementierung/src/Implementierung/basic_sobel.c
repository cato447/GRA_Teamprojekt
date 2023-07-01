#include "basic_sobel.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>

const int8_t M_v[3][3] = { { 1, 0, -1 },
                           { 2, 0, -2 },
                           { 1, 0, -1 } };

const int8_t M_h[3][3] = { { 1, 2, 1 },
                           { 0, 0, 0 },
                           { -1, -2, -1 } };

void sobel(const uint8_t* img_in, size_t width, size_t height, uint8_t* img_out) {
    for (size_t x = 1; x < width-1; ++x) {
        for (size_t y = 1; y < height-1; ++y) {
            int32_t A_v_R = 0;
            int32_t A_h_R = 0;

            int32_t A_v_G = 0;
            int32_t A_h_G = 0;

            int32_t A_v_B = 0;
            int32_t A_h_B = 0;

            for (int8_t i = -1; i <= 1; i++)
            {
                for (int8_t j = -1; j <= 1; j++)
                {
                    A_v_R += (int32_t)(M_v[1 + i][1 + j] * (colorOfPixel(img_in, width, x + i, y + j, RED)));
                    A_h_R += (int32_t)(M_h[1 + i][1 + j] * (colorOfPixel(img_in, width, x + i, y + j, RED)));

                    A_v_G += (int32_t)(M_v[1 + i][1 + j] * (colorOfPixel(img_in, width, x + i, y + j, GREEN)));
                    A_h_G += (int32_t)(M_h[1 + i][1 + j] * (colorOfPixel(img_in, width, x + i, y + j, GREEN)));

                    A_v_B += (int32_t)(M_v[1 + i][1 + j] * (colorOfPixel(img_in, width, x + i, y + j, BLUE)));
                    A_h_B += (int32_t)(M_h[1 + i][1 + j] * (colorOfPixel(img_in, width, x + i, y + j, BLUE)));
                }
            }

//            int32_t A_R = sqrt((A_v_R * A_v_R) + (A_h_R * A_h_R));
//            int32_t A_G = sqrt((A_v_G * A_v_G) + (A_h_G * A_h_G));
//            int32_t A_B = sqrt((A_v_B * A_v_B) + (A_h_B * A_h_B));
            int32_t A_R = abs(A_v_R) + abs(A_h_R);
            int32_t A_G = abs(A_v_G) + abs(A_h_G);
            int32_t A_B = abs(A_v_B) + abs(A_h_B);
            setPixelAt(img_out, width, x, y, A_R > 255 ? 255 : A_R, A_G  > 255 ? 255 : A_G, A_B  > 255 ? 255 : A_B);
        }
    }
}

uint8_t colorOfPixel(const uint8_t* img, size_t width, size_t x, size_t y, enum Color color) {
    // Pixel are 24 bit wide to compensate this we multiply x and y by 3
    return *(img + width * (y * 3) + x * 3 + color);
}

void setPixelAt(uint8_t* img, size_t width, size_t x, size_t y, uint8_t red, uint8_t green, uint8_t blue) {
    // Pixel are 24 bit wide to compensate this we multiply x and y by 3
    *(img + width * (y * 3) + x * 3 + BLUE) = blue;
    *(img + width * (y * 3) + x * 3 + GREEN) = green;
    *(img + width * (y * 3) + x * 3 + RED) = red;
}