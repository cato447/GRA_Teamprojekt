//POSIX function fileno() not part of C17 standard
#define _POSIX_C_SOURCE 199309L

#include "IO_tools.h"

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

/*
Returns a pointer to the data buffer of the file read at parameter "path" or a null pointer on failure.
Sets "bufSize" accordingly on successful read.
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

    void* fileBuf = malloc(statbuf.st_size);
    if (fileBuf == NULL) {
        fprintf(stderr, "Failed allocating memory for fileBuf\n");
        fclose(file);
        return NULL;
    }

    if (fread(fileBuf, 1, statbuf.st_size, file) != (size_t) statbuf.st_size) {
        fprintf(stderr, "Failed reading file data at %s\n", path);
        fclose(file);
        free(fileBuf);
        return NULL;
    }
    
    *bufSize = statbuf.st_size;
    fclose(file);
    return fileBuf;
}

/*
Writes "bufSize" bytes from buffer at parameter "buf" to file at location "path", creates files if doesn't exist prior.
*/
int writeFile(const char* path, char* buf, size_t bufSize) {
    FILE* file = fopen(path, "wb+");
    if (!file) {
        fprintf(stderr, "Failed trying to create/overwrite file at \"%s\": %s\n", path, strerror(errno));
        return 1;
    }

    if (fwrite(buf, 1, bufSize, file) != bufSize) {
        fprintf(stderr, "Failed writing buffer data to %s\n", path);
        fclose(file);
        return 1;
    }

    fclose(file);
    return 0;
}
