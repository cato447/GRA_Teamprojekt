#include <stdint.h>
#include <stddef.h>

void cuda_sobel(uint8_t* img_in, size_t width, size_t height, uint8_t* img_out);
