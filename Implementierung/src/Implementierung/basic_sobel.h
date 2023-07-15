#include <stdint.h>
#include <stddef.h>

enum Color { BLUE, GREEN, RED };

uint8_t color_of_pixel(const uint8_t *img, size_t width, size_t x, size_t y, enum Color color);
void set_pixel_at(uint8_t *img, size_t width, size_t x, size_t y, uint8_t red, uint8_t green, uint8_t blue);

void sobel(const uint8_t *img_in, size_t width, size_t height, uint8_t *img_out);
void sobel_graysc(const uint8_t *img_in, size_t width, size_t height, uint8_t *img_out);
