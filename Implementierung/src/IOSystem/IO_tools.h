#ifndef _IO_TOOLS_H_
#define _IO_TOOLS_H_

#include <stddef.h>

char *read_file(const char *path, size_t *buf_size);
int write_file(const char *path, char *buf, size_t buf_size);

#endif
