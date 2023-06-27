#include "IOtools.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

/*
Returns a pointer to the data buffer of the file read at parameter "path" and sets "bufSize" accordingly on successful read.
*/
void* readFile(const char* path, size_t* bufSize) {
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
    if (!S_ISREG(statbuf.st_mode)) {
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

    if (fread(buf, 1, statbuf.st_size, file) != statbuf.st_size) {
        fprintf(stderr, "Error: failed reading file data at %s\n", path);
        fclose(file);
        free(buf);
        return NULL;
    }
    
    *bufSize = statbuf.st_size;
    fclose(file);
    return buf;
}

void writeFile(const char* path, void* buffer, size_t bufSize) {
    FILE* file = fopen(path, "wb+");
    if (!file) {
        fprintf(stderr, "Error trying to create/overwrite file at \"%s\": %s\n", path, strerror(errno));
        return;
    }

    if (fwrite(buffer, 1, bufSize, file) != bufSize) {
        fprintf(stderr, "Error: failed writing buffer data to %s\n", path);
    }

    fclose(file);
}
