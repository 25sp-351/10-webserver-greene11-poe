#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "response.h"
#include "header.h"
#include "logging.h"


static void response_add_header(Response* rsp, char* key, char* value);

static void response_free(Response* rsp);

static void response_print(Response* rsp);

// true = success, false = failure
static bool response_send_to_fd(Response* rsp, int fd);

// for null-terminated strings
static void response_set_body(Response* rsp, char* body);

Response* response_create(void) {
    Response* rsp = malloc(sizeof(Response));
    rsp->status_code = 0;
    rsp->version = strdup("HTTP/1.1");
    rsp->headers = new_vec(sizeof(Header));
    rsp->body = NULL;
    rsp->body_length = 0;

    rsp->add_header = response_add_header;
    rsp->print = response_print;
    rsp->free = response_free;
    rsp->send_to_fd = response_send_to_fd;
    rsp->set_body = response_set_body;

    return rsp;
}

static void response_add_header(Response* rsp, char* key, char* value) {
    add_header(rsp->headers, key, value, true);
}

#define FREE_IF_NOT_NULL(ptr) if (ptr) { free(ptr); }

static void response_free(Response* rsp) {
    if (rsp == NULL) {
        return;
    }
    FREE_IF_NOT_NULL(rsp->version);
    headers_free(rsp->headers);
    FREE_IF_NOT_NULL(rsp->body);
}


static void response_print(Response* rsp) {
    log("vvv Response vvv");
    log("%s %d", rsp->version, rsp->status_code);
    headers_write(1, rsp->headers);
    log("^^^ Response ^^^");
}

char *http_code_to_string(int code) {
    switch (code) {
        case 200:
            return "OK";
        case 400:
            return "Bad Request";
        case 404:
            return "Not Found";
        case 500:
            return "Internal Server Error";
        default:
            return "Unknown";
    }
}

static bool response_send_to_fd(Response* rsp, int fd) {
    int result;
    result = dprintf(fd, "%s %d %s\r\n", rsp->version, rsp->status_code, http_code_to_string(rsp->status_code));
    if (result <= 0) {
        return false;
    }
    result = headers_write(fd, rsp->headers);
    if (result <= 0) {
        return false;
    }
    result = dprintf(fd, "\r\n");
    if (result <= 0) {
        return false;
    }
    if (rsp->body_length > 0) {
        result = write(fd, rsp->body, rsp->body_length);
        if (result <= 0) {
            return false;
        }
    }
    return true;
}

static void response_set_body(Response* rsp, char* body) {
    if (rsp->body) {
        free(rsp->body);
    }
    rsp->body = strdup(body);
    rsp->body_length = strlen(body);
}

