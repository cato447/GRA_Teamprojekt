#include "test_functionality.h"

#include <stdio.h>
#include <stdlib.h>

#include "IO_tools.h"
#include "bmp_parser.h"

size_t loadPicture(char *path, uBMPImage *img) {
    void *buffer;
    size_t buffer_size;
    fprintf(stdout, "Loading BMP File: %s\n", path);
    buffer = readFile(path, &buffer_size);

    if (buffer == NULL) {
        fprintf(stderr, "Couldn't read BMP File\n");
        return 0;
    }
    if (img == NULL) {
        fprintf(stderr, "img was not initialized\n");
        return 0;
    }
    if (bmpToArray(buffer, buffer_size, img) == 1) {
        fprintf(stderr, "Couldn't parse BMP file");
        free(buffer);
        return 0;
    }
    free(buffer);
    return buffer_size;
}
