#include "test_functionality.h"

#include <stdio.h>
#include <stdlib.h>

#include "IO_tools.h"
#include "bmp_parser.h"

size_t load_picture(char *path, s_image *img) {
    void *buffer;
    size_t buffer_size;
    buffer = read_file(path, &buffer_size);

    if (buffer == NULL) {
        fprintf(stderr, "Couldn't read BMP File\n");
        return 0;
    }
    if (bmp_to_array(buffer, buffer_size, img) == 1) {
        fprintf(stderr, "Couldn't parse BMP file\n");
        free(buffer);
        return 0;
    }
    free(buffer);
    return buffer_size;
}

size_t load_picture_graysc(char *path, s_image *img) {
    void *buffer;
    size_t buffer_size;
    buffer = read_file(path, &buffer_size);

    if (buffer == NULL) {
        fprintf(stderr, "Couldn't read BMP File\n");
        return 0;
    }
    if (bmp_to_array_graysc(buffer, buffer_size, img) == 1) {
        fprintf(stderr, "Couldn't parse BMP file\n");
        free(buffer);
        return 0;
    }
    free(buffer);
    return buffer_size;
}
