#ifndef OPTO_FS_H_
#define OPTO_FS_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define PATH_MAX 4096

typedef struct {
    char* buffer;
    size_t length;
} string_t;

string_t read_file_contents(const char *filename);
char* verify_path(char* response_path, const char* exe_dir, const char* request_path);

#endif