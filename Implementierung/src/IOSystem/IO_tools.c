#include "IO_tools.h"

//POSIX function fileno() not part of C17 standard
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

#include "bmp_parser.h"

/*
Returns a pointer to the data buffer of the file read at parameter "path" and sets "bufSize" accordingly on successful read.
*/
char* readFile(const char* path, size_t* bufSize) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Error trying to open file at \"%s\": %s\n", path, strerror(errno));
        return NULL;
    }

    struct stat statbuf;
    
    if (fstat(fileno(file), &statbuf)) {
        fprintf(stderr, "Error trying to check stats of file at %s: %s\n", path, strerror(errno));
        fclose(file);
        return NULL;
    }
    if (!S_ISREG(statbuf.st_mode) || (statbuf.st_size <= 0)) {
        fprintf(stderr, "Error: file at %s isn't a regular file\n", path);
        fclose(file);
        return NULL;
    }

    void* buf = malloc(statbuf.st_size);
    if (!buf) {
        fprintf(stderr, "Error: failed allocating memory for file buffer\n");
        fclose(file);
        return NULL;
    }

    if (fread(buf, 1, statbuf.st_size, file) != (size_t) statbuf.st_size) {
        fprintf(stderr, "Error: failed reading file data at %s\n", path);
        fclose(file);
        free(buf);
        return NULL;
    }
    
    *bufSize = statbuf.st_size;
    fclose(file);
    return buf;
}

size_t loadPicture(char *path, uBMPImage *img) {
    void *buffer;
    size_t buffer_size;
    fprintf(stdout, "Loading BMP File: %s for test data\n", path);
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



/*
Writes "bufSize" bytes from buffer at parameter "buf" to file at location "path", creates files if doesn't exist prior.
*/
void writeFile(const char* path, char* buf, size_t bufSize) {
    FILE* file = fopen(path, "wb+");
    if (!file) {
        fprintf(stderr, "Error trying to create/overwrite file at \"%s\": %s\n", path, strerror(errno));
        return;
    }

    if (fwrite(buf, 1, bufSize, file) != bufSize) {
        fprintf(stderr, "Error: failed writing buffer data to %s\n", path);
    }

    fclose(file);
}
