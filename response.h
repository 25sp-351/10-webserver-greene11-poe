#ifndef RESPONSE_H
#define RESPONSE_H

#include "header.h"
#include "vec.h"

// char* are malloc'd strings, except for body
// body is malloc'd but may not be a string
typedef struct rsp {
    int status_code;
    char* version;
    Vec headers;
    char* body;
    int body_length;

    void (*add_header)(struct rsp* rsp, char* key, char* value);
    void (*print)(struct rsp* rsp);
    void (*free)(struct rsp* rsp);
    bool (*send_to_fd)(struct rsp* rsp, int fd);
    void (*set_body)(struct rsp* rsp, char* body);
} Response;

Response* response_create(void);

#endif
