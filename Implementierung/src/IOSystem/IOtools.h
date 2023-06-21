#include <stddef.h>

void* readFile (const char* path, size_t* bufSize);

void writeFile (const char* path, void* buffer, size_t bufSize);
