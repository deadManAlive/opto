#ifndef OPTO_HTTP_H_
#define OPTO_HTTP_H_

#include "fs.h"

typedef string_t response_t;

enum Status {
    OK = 200,
    NOT_FOUND = 404
};

response_t generate_response(enum Status status, const char* header, const string_t* response_body);


#endif