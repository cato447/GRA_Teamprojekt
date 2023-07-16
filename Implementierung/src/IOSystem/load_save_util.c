#include "load_save_util.h"

#include <stdio.h>
#include <stdlib.h>

#include "IO_tools.h"
#include "bmp_parser.h"

s_image *load_image(const char *path, int in_graysc) {
    size_t buf_size;
    char *buf = read_file(path, &buf_size);
    if (buf == NULL) {
        fprintf(stderr, "Failed to read image from \"%s\"\n", path);
        return NULL;
    }

    s_image *img_buf = malloc(sizeof(s_image));
    if (img_buf == NULL) {
        fprintf(stderr, "Failed allocating memory for img_buf\n");
        free(buf);
        return NULL;
    }

    if (in_graysc ? bmp_to_array_graysc(buf, buf_size, img_buf) : bmp_to_array(buf, buf_size, img_buf)) {
        fprintf(stderr, "Failed to parse image from \"%s\"\n", path);
        free(buf);
        free(img_buf);
        return NULL;
    }

    free(buf);
    return img_buf;
}

int save_image(s_image *img_buf, int in_graysc, const char *path) {
    size_t buf_size;
    char *buf = in_graysc ? array_to_bmp_graysc(img_buf, &buf_size) : array_to_bmp(img_buf, &buf_size);
    if (buf == NULL) {
        fprintf(stderr, "Failed to generate BMP file date from image");
        return 1;
    }

    if (write_file(path, buf, buf_size)) {
        fprintf(stderr, "Failed to save image to \"%s\"\n", path);
        free(buf);
        return 1;
    }

    free(buf);
    return 0;
}
