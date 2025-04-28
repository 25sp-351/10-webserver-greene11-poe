#ifndef REQUEST_H
#define REQUEST_H

#include "header.h"
#include "vec.h"

typedef struct hreq {
    char* method;
    char* path;
    char* version;
    Vec headers;
    char* body;
    int body_length;

    void (*print)(struct hreq* req);
    void (*free)(struct hreq* req);

} Request;

Request* request_read_from_fd(int fd);

#endif
