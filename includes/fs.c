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