#include "bmp_parser.h"

#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

#define BMP_HEADER_SIGNATURE 0x4d42

typedef struct pixel_3 {
    uint8_t b;
    uint8_t g;
    uint8_t r;
} pixel_3;
typedef struct pixel_1 {
    uint8_t v;
} pixel_1;

//INPUT STUFF

struct __attribute__((__packed__)) bmp_header_parsing {
    uint16_t signature;
    uint32_t file_size;
    uint32_t _;
    uint32_t data_offset;
    uint32_t info_header_size;
    union img_size {
        struct __attribute__((__packed__)) info {
            int32_t px_width;
            int32_t px_height;
        } info;
        struct __attribute__((__packed__)) core {
            uint16_t px_width;
            uint16_t px_height;
            uint16_t _0;
            uint16_t _1;
        } core;
    } img_size; 
 };

static const char *parseHeader(const char *buf, size_t buf_size, size_t *_px_data_width, uint32_t *_px_width, uint32_t *_px_height, int *_neg_height) {
    if (buf_size < sizeof(struct bmp_header_parsing)) {
        fprintf(stderr, "Error: file too small to contain relevant BMP header information\n");
        return NULL;
    }

    struct bmp_header_parsing *header = (struct bmp_header_parsing *) buf;

    if (header->signature != BMP_HEADER_SIGNATURE) {
        fprintf(stderr, "Error: Input not a BMP file (header mismatch)\n");
        fprintf(stderr, "  Expected [0x4d42], got [0x%x]\n", header->signature);
        return NULL;
    }

    if (header->file_size != buf_size) {
        printf("Warning: size specified in file header not matching real file size, file may be corrupted\n");
        printf("  Expected: %uB, got %luB\n", header->file_size, buf_size);
    }

    // Header of type BITMAPCOREHEADER, using only 32 instead of 64 bits to represent size
    if (header->info_header_size == 12) {
        *_px_width = header->img_size.core.px_width;
        *_px_height = header->img_size.core.px_height;        
    } else {
        if (header->img_size.info.px_width < 0) {
            printf("Warning: negative width value in file header (%d), using absolute value (%d)\n", header->img_size.info.px_width, -header->img_size.info.px_width);
            *_px_width = -header->img_size.info.px_width;
        } else {
            *_px_width = header->img_size.info.px_width;
        }

        if (header->img_size.info.px_height < 0) {
            *_px_height = -header->img_size.info.px_height;
            *_neg_height = 1;
        } else {
            *_px_height = header->img_size.info.px_height;
            *_neg_height = 0;
        }
    }

    size_t byte_width = *_px_width * sizeof(pixel_3);
    *_px_data_width = (byte_width & 0x3) ? ((byte_width & ~0x3) + 4) : byte_width;

    if ((header->data_offset + *_px_data_width * (*_px_height)) > buf_size) {
        fprintf(stderr, "Error: file size doesn't match file info\n");
        fprintf(stderr, "  Expected (at max) %luB, got %luB\n", buf_size, header->data_offset + *_px_data_width * (*_px_height));
        return NULL;
    }

    return buf + header->data_offset;
}

/*
Returns 0 on success and 1 on failure.
Sets "width", "height", "px_array_size" and "px_array" of "_bmp_img_buf" to an unpadded copy of the of the parameter "bmpFile".
Pixelarray starts in bottom left of picture.
*/
int bmp_to_array(const char *buf, size_t buf_size, s_image *_bmp_img_buf) {
    size_t px_data_rowlength;
    uint32_t px_width;
    uint32_t px_height;
    int neg_height = 0;
    const char *buf_px_data = parseHeader(buf, buf_size, &px_data_rowlength, &px_width, &px_height, &neg_height);
    if (buf_px_data == NULL) {
        fprintf(stderr, " → Failed parsing the file header\n");
        return 1;
    }

    size_t px_array_rowlength = px_width * sizeof(pixel_3);

    size_t px_array_size = px_array_rowlength * px_height;
    uint8_t *px_array = calloc(px_array_size, sizeof(uint8_t));
    if (px_array == NULL) {
        fprintf(stderr, "Failed allocating memory for px_array\n");
        return 1;
    }

    uint8_t *px_array_end = px_array + px_array_rowlength * px_height;

    if (neg_height) {
        for (uint8_t *dest = px_array_end - px_array_rowlength; dest >= px_array; dest -= px_array_rowlength, buf_px_data += px_data_rowlength) {
            memcpy(dest, buf_px_data, px_array_rowlength);
        }
    } else {
        for (uint8_t *dest = px_array; dest < px_array_end; dest += px_array_rowlength, buf_px_data += px_data_rowlength) {
            memcpy(dest, buf_px_data, px_array_rowlength);
        }
    }

    _bmp_img_buf->px_array = px_array;
    _bmp_img_buf->px_width = px_width;
    _bmp_img_buf->px_height = px_height;
    _bmp_img_buf->px_array_size = px_array_size;

    return 0;
}

//Scaling factors: https://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.709-6-201506-I!!PDF-E.pdf
#define R_SCALING .2126f
#define G_SCALING .7152f
#define B_SCALING .0722f
/*
Returns 0 on success and 1 on failure.
Sets "width", "height" and "px_array" of "_bmp_img_buf" to an unpadded copy of the of the parameter "bmpFile".
Pixelarray starts in bottom left of picture.
*/
int bmp_to_array_graysc (const char *buf, size_t buf_size, s_image *_bmp_img_buf) {
    size_t px_data_rowlength;
    uint32_t px_width;
    uint32_t px_height;
    int neg_height = 0;
    const char *buf_px_data = parseHeader(buf, buf_size, &px_data_rowlength, &px_width, &px_height, &neg_height);
    if (buf_px_data == NULL) {
        fprintf(stderr, " → Failed parsing the file header\n");
        return 1;
    }

    size_t px_array_rowlength = px_width * sizeof(pixel_1);

    size_t px_array_size = px_array_rowlength * px_height;
    uint8_t *px_array = calloc(px_array_size, sizeof(uint8_t));
    if (px_array == NULL) {
        fprintf(stderr, "Failed allocating memory for px_array\n");
        return 1;
    }

    uint8_t *px_array_end = px_array + px_array_rowlength * px_height;

    if (neg_height) {
        for (uint8_t *dest = px_array_end - px_array_rowlength; dest >= px_array; dest -= px_array_rowlength, buf_px_data += px_data_rowlength) {
            pixel_3 *px_data_arr = (pixel_3 *) buf_px_data;
            for (uint32_t i = 0; i < px_width; ++i) {
                ((pixel_1 *) dest)[i].v = px_data_arr[i].r * R_SCALING + px_data_arr[i].g * G_SCALING + px_data_arr[i].b * B_SCALING;
            }
        }
    } else {
        for (uint8_t *dest = px_array; dest < px_array_end; dest += px_array_rowlength, buf_px_data += px_data_rowlength) {
            pixel_3 *px_data_arr = (pixel_3 *) buf_px_data;
            for (uint32_t i = 0; i < px_width; ++i) {
                ((pixel_1 *) dest)[i].v = px_data_arr[i].r * R_SCALING + px_data_arr[i].g * G_SCALING + px_data_arr[i].b * B_SCALING;
            }
        }
    }

    _bmp_img_buf->px_array = px_array;
    _bmp_img_buf->px_width = px_width;
    _bmp_img_buf->px_height = px_height;
    _bmp_img_buf->px_array_size = px_array_size;

    return 0;
}


//OUTPUT STUFF
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40

struct __attribute__((__packed__)) bmp_header_writing {
    uint16_t signature;
    uint32_t file_size;
    uint32_t _0;
    uint32_t data_offset;
    uint32_t info_header_size;
    int32_t px_width;
    int32_t px_height;
    uint16_t planes;
    uint16_t bit_depth;
    uint32_t _1;
    uint32_t _2;
    int32_t _3;
    int32_t _4;
    uint32_t _5;
    uint32_t _6;
};

struct __attribute__((__packed__)) color_table_entry {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t _;
};

static void copy_pixel_data(const uint8_t *px_array, size_t px_array_size, uint32_t px_array_rowlength, uint32_t px_data_rowlength, char *_px_data) {
    const uint8_t *px_array_end = px_array + px_array_size;

    while (px_array < px_array_end) {
        memcpy(_px_data, px_array, px_array_rowlength);
        px_array += px_array_rowlength;
        _px_data += px_data_rowlength;
    }
}

/*
Return a pointer to a buffer containing a complete BMP image.
Writes size of buffer to parameter "_size".
*/
char *array_to_bmp(const s_image *bmp_img, size_t *_size) {
    uint32_t px_array_rowlength = bmp_img->px_width * sizeof(pixel_3);
    uint32_t buf_px_rowlength = (px_array_rowlength & 0x3) ? ((px_array_rowlength & ~0x3) + 4) : px_array_rowlength;

    *_size = sizeof(struct bmp_header_writing) + buf_px_rowlength * (bmp_img->px_height);
    char *buf = calloc(*_size, sizeof(uint8_t));
    if (buf == NULL) {
        fprintf(stderr, "Failed allocating memory for buf\n");
        return NULL;
    }

    struct bmp_header_writing *header = (struct bmp_header_writing *) buf;
    header->signature = BMP_HEADER_SIGNATURE;
    header->file_size = *_size;
    header->data_offset = sizeof(struct bmp_header_writing);
    header->info_header_size = INFO_HEADER_SIZE;
    header->px_width = bmp_img->px_width;
    header->px_height = bmp_img->px_height;
    header->planes = 1;
    header->bit_depth = 8 * sizeof(pixel_3);

    copy_pixel_data(bmp_img->px_array, bmp_img->px_array_size, px_array_rowlength, buf_px_rowlength, buf + sizeof(struct bmp_header_writing));
    return buf;
}

/*
Return a pointer to a buffer containing a complete grayscale BMP image.
Writes size of buffer to parameter "_size".
*/
char *array_to_bmp_graysc(const s_image *bmp_img, size_t * _size) {
    uint32_t px_array_rowlength = bmp_img->px_width * sizeof(pixel_1);
    uint32_t buf_px_rowlength = (px_array_rowlength & 0x3) ? ((px_array_rowlength & ~0x3) + 4) : px_array_rowlength;

    *_size = sizeof(struct bmp_header_writing) + 256 * sizeof(struct color_table_entry) + buf_px_rowlength * (bmp_img->px_height);
    char *buf = calloc(*_size, sizeof(uint8_t));
    if (buf == NULL) {
        fprintf(stderr, "Failed allocating memory for buf\n");
        return NULL;
    }

    struct bmp_header_writing *header = (struct bmp_header_writing *) buf;
    header->signature = BMP_HEADER_SIGNATURE;
    header->file_size = *_size;
    header->data_offset = sizeof(struct bmp_header_writing) + 256 * sizeof(struct color_table_entry);
    header->info_header_size = INFO_HEADER_SIZE;
    header->px_width = bmp_img->px_width;
    header->px_height = bmp_img->px_height;
    header->planes = 1;
    header->bit_depth = 8 * sizeof(pixel_1);

    struct color_table_entry *color_table = (struct color_table_entry *) (buf + sizeof(struct bmp_header_writing));
    for (int i = 0; i < 256; ++i) {
        color_table[i].r = i;
        color_table[i].g = i;
        color_table[i].b = i;
    }

    copy_pixel_data(bmp_img->px_array, bmp_img->px_array_size,
                    px_array_rowlength, buf_px_rowlength, buf + 256 * sizeof(struct color_table_entry) + sizeof(struct bmp_header_writing));
    return buf;
}
