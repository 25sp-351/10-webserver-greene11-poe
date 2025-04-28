#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "header.h"
#include "line.h"
#include "logging.h"
#include "request.h"

bool read_request_line(Request* req, int fd);
bool read_body(Request* req, int fd);
void request_free(Request* req);
void request_print(Request* req);

Request* new_request()
{
    Request* req = malloc(sizeof(Request));
    req->method = NULL;
    req->path = NULL;
    req->version = NULL;
    req->headers = NULL;
    req->body = NULL;
    req->body_length = 0;

    req->print = request_print;
    req->free = request_free;

    return req;
}
Request* request_read_from_fd(int fd)
{
    // printf("Reading request from fd %d\n", fd);

    Request* req = new_request();

    if (read_request_line(req, fd) == false) {
        log("Failed to read request line");
        req->free(req);
        return NULL;
    }
    // read_headers also eats the second CRLF
    req->headers = read_headers(fd);

    if (req->headers == NULL) {
        printf("Failed to read headers\n");
        req->free(req);
        return NULL;
    }
    if (read_body(req, fd) == false) {
        log("Failed to read body");
        req->free(req);
        return NULL;
    }

    req->print(req);
    ;
    return req;
}

void request_print(Request* req)
{
    printf("vvv Request vvv\n");
    if (req->method) {
        printf("Method: %s\n", req->method);
    }
    if (req->path) {
        printf("Path: %s\n", req->path);
    }
    if (req->version) {
        printf("Version: %s\n", req->version);
    }
    headers_print(req->headers);

    printf("^^^ Request ^^^\n");
}

#define FREE_IF_NOT_NULL(ptr)                                                  \
    if (ptr) {                                                                 \
        free(ptr);                                                             \
    }

void request_free(Request* req)
{
    printf("request_free\n");
    if (req == NULL) {
        return;
    }
    FREE_IF_NOT_NULL(req->method);
    FREE_IF_NOT_NULL(req->path);
    FREE_IF_NOT_NULL(req->version);
    FREE_IF_NOT_NULL(req->body);

    headers_free(req->headers);

    free(req);
}

bool read_request_line(Request* req, int fd)
{
    // printf("read_request_line\n");
    char* line;

    // accept empty lines before the request line
    while (1) {
        line = read_http_line(fd);
        if (line == NULL) {
            return false;
        }
        if (strlen(line) > 0) {
            break;
        }
        free(line);
    }

    req->method = malloc(strlen(line) + 1);
    req->path = malloc(strlen(line) + 1);
    req->version = malloc(strlen(line) + 1);
    int length_parsed;
    int number_parsed;

    // NOLINTNEXTLINE
    number_parsed = sscanf(line, "%s %s %s%n", req->method, req->path,
        req->version, &length_parsed);

    if (number_parsed != 3 || length_parsed != strlen(line)) {
        printf("%d parsed, %d length parsed, %lu line len\n", number_parsed,
            length_parsed, strlen(line));
        printf("Failed to parse request line\n");
        free(line);
        return false;
    }

    if (strcmp(req->method, "GET") != 0 && strcmp(req->method, "POST") != 0) {
        printf("Invalid method: %s\n", req->method);
        free(line);
        return false;
    }

    return true;
}
bool read_body(Request* req, int fd)
{
    // printf("read_body\n");
    req->body = NULL;
    req->body_length = 0;

    char* body_length_str = get_header_value(req->headers, "Content-Length");
    if (body_length_str == NULL) {
        // printf("No content length\n");
        return true;
    }

    // NOLINTNEXTLINE
    if (sscanf(body_length_str, "%d", &req->body_length) != 1) {
        printf("Failed to parse content length\n");
        return false;
    }

    if (req->body_length == 0) {
        // printf("No body\n");
        return true;
    }

    req->body = malloc(req->body_length + 1);
    int bytes_read = read(fd, req->body, req->body_length);
    if (bytes_read != req->body_length) {
        printf("Failed to read body\n");
        free(req->body);
        req->body = NULL;
        req->body_length = 0;
        return false;
    }
    // make sure it's can be string
    req->body[req->body_length] = '\0';

    return true;
}
