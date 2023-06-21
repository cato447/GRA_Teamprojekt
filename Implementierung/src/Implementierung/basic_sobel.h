#include <stdint.h>
#include <stddef.h>

enum Color { RED, GREEN, BLUE };

uint8_t colorOfPixel(const uint8_t* img, size_t width, size_t x, size_t y, enum Color color);
void setPixelAt(uint8_t* img, size_t width, size_t x, size_t y, uint8_t red, uint8_t green, uint8_t blue);

void sobel(const uint8_t* img_in, size_t width, size_t height, uint8_t* img_out);
