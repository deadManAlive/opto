#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "fs.h"

string_t read_file_contents(const char *filename)
{
    FILE *file;
    string_t string = { NULL, 0 };
    
    // Open the file for reading
    file = fopen(filename, "rb");
    if (!file) {
        printf("Failed to open file.\n");
        return string;
    }
    
    // Get the file size
    fseek(file, 0, SEEK_END);
    string.length = ftell(file);
    rewind(file);
    
    // Allocate memory for the buffer
    string.buffer = (char*)malloc(string.length + 1);
    if (!string.buffer) {
        printf("Failed to allocate memory for buffer.\n");
        string.length = 0;
        return string;
    }
    
    // Read the file contents into the buffer
    if (fread(string.buffer, string.length, 1, file) != 1) {
        printf("Failed to read file.\n");
        free(string.buffer);
        string.length = 0;
        return string;
    }
    
    // Add null terminator to end of buffer
    string.buffer[string.length] = '\0';
    
    // Clean up and return buffer
    fclose(file);
    return string;
}

char* verify_path(char* response_path, const char* exe_dir, const char* request_path) {
    // get requested path
    snprintf(response_path, PATH_MAX, "%s/routes%s", exe_dir, request_path);

    printf("Requesting [%s]...\n", response_path);

    if (access(response_path, F_OK) != 0) {
        strcat(response_path, ".html");
        printf("Trying [%s]...\n", response_path);

        if (access(response_path, F_OK) != 0) {
            perror("error reading path");
            return NULL;
        }
    }

    struct stat path_stat;
    if (stat(response_path, &path_stat) == 0) {
        if (S_ISREG(path_stat.st_mode)) {
            printf("[%s] is regular file\n", response_path);
            return response_path;
        }
        else if (S_ISDIR(path_stat.st_mode)) {
            printf("[%s] is a directory\n", response_path);
            char index[] = "index.html";
            if (response_path[strlen(response_path) - 1] != '/') {
                strcat(response_path, "/");
            }
            strcat(response_path, index);
            printf("accessing [%s]...\n", response_path);
            return response_path;
        }
        else {
            perror("non regular path");
            return NULL;
        }
    }

    return NULL;
}