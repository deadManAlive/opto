#ifndef OPTO_FS_H_
#define OPTO_FS_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char* buffer;
    size_t length;
} string_t;

string_t read_file_contents(const char *filename);

#endif