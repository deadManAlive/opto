#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "http.h"

response_t generate_response(enum Status status, const char* header, const string_t* response_body) {
    // NOTE: accepting and checking NULL head/body is hellish, consider NOT
    response_t result = {NULL, 0};

    // protocol + status code
    char* phead = malloc(32 + (header?strlen(header):0));
    if (!phead) {
        perror("error in generating response");
        return result;
    }
    memcpy(phead, "HTTP/1.1 ", 9);

    switch(status) {
        case OK:
            memcpy(phead + 9, "200 OK\r\n", 8);
            phead[9 + 8] = '\0';
            break;
        case NOT_FOUND:
            memcpy(phead + 9, "404 Not Found\r\n", 15);
            phead[9 + 15] = '\0';
            break;
    }

    // append header, if any
    size_t phead_length = strlen(phead);
    size_t header_length = (header?strlen(header):0);
    if (header) {
        memcpy(phead + phead_length, header, header_length);
        memcpy(phead + phead_length + header_length, "\r\n", 2);
    }

    //append header-body delimiter (\r\n)
    memcpy(phead + phead_length + header_length + (header?2:0), "\r\n", 2);
    phead[phead_length + header_length + (header?2:0) + 2] = '\0';

    size_t total_header_length = phead_length + header_length + (header?4:2);

    //append to result
    size_t total_response_length = total_header_length + (response_body?response_body->length:0);

    result.buffer = malloc(total_response_length + 1);
    if (!result.buffer) {
        perror("error in generating response");
        return result;
    }

    memcpy(result.buffer, phead, total_header_length);
    if (response_body) { memcpy(result.buffer + total_header_length, response_body->buffer, response_body->length); }
    
    result.buffer[total_response_length] = '\0';
    result.length = total_response_length;

    return result;
}