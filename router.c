#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "request.h"
#include "response.h"
#include "router.h"

// indicate that we intentionally don't use a variable
#define UNUSED(x) (void*)x

Response* get_stats_handler(Request* req);
Response* get_static_handler(Request* req);
Response* get_calc_handler(Request* req);
Response* no_resource_handler(Request* req);
Response* internal_error(Request* req);
Response* bad_request_handler(Request* req);

typedef struct {
    char* method;
    char* path;
    bool full_match;
    Response* (*handler)(Request*);
} Route;

static Route routes[] = {
    { "GET", "/stats", true, get_stats_handler },
    { "GET", "/static/", false, get_static_handler },
    { "GET", "/calc/", false, get_calc_handler },
    { "GET", "", false, no_resource_handler },
    { NULL, NULL, false, NULL } };

Response* route_request(Request* req)
{
    for (Route* candidate = routes; candidate->method != NULL; candidate++) {
        if (strcmp(candidate->method, req->method))
            continue;

        if (candidate->full_match && strcmp(candidate->path, req->path))
            continue;

        if (!candidate->full_match
            && strncmp(candidate->path, req->path, strlen(candidate->path)))
            continue;

        return (*candidate->handler)(req);
    }
    return NULL;
}

Response* get_stats_handler(Request* req)
{
    UNUSED(req);
    Response* res = response_create();
    res->status_code = 200;
    res->set_body(res, "Stats go here\n");
    return res;
}
typedef struct {
    char* extension;
    char* content_type;
    bool as_file;
} ContentType;

ContentType content_types[] = {
    { ".html", "text/html", false },
    { ".css", "text/css", false },
    { ".js", "application/javascript", false },
    { ".jpg", "image/jpeg", true },
    { ".png", "image/png", true },
    { ".gif", "image/gif", true },
    { ".md", "text/markdown", true }, { NULL, NULL, false } };

ContentType default_content_type = { "", "text/plain", true };

ContentType* content_type_for_file(char* path)
{
    char* extension = strrchr(path, '.');
    if (extension == NULL)
        return &default_content_type;
    for (ContentType* ct = content_types; ct->extension != NULL; ct++) {
        if (strcmp(ct->extension, extension) == 0)
            return ct;
    }
    return &default_content_type;
}
char *strendswith(const char *str, const char *suffix)
{
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > str_len)
        return NULL;
    return strncmp(str + str_len - suffix_len, suffix, suffix_len) == 0 ? (char *)str + str_len - suffix_len : NULL;
}

Response* get_static_handler(Request* req)
{

    // can't walk up the dir path.
    if (strstr(req->path, "../") || strstr(req->path, "/.."))
        return bad_request_handler(req);

    int fd = open(req->path + 1, O_RDONLY);
    if (fd == -1)
        return no_resource_handler(req);

    struct stat st;

    // gotta exist
    if (fstat(fd, &st) == -1) {
        return no_resource_handler(req);
    }
    // only regular files
    if (!S_ISREG(st.st_mode)) {
        return bad_request_handler(req);
    }

    int file_size = st.st_size;

    Response* res = response_create();
    res->body = malloc(file_size);
    res->body_length = read(fd, res->body, file_size);
    close(fd);
    if (res->body_length != file_size) {
        res->free(res);
        return internal_error(req);
    }
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%d", res->body_length);res->add_header(res, "Content-Length", buffer);
    ContentType* ct = content_type_for_file(req->path);
    if (ct->as_file) {
        res->add_header(res, "Content-Disposition", "inline");

        char* slashPos = strrchr(req->path, '/');
        if (!slashPos) {
            res->free(res);
            return internal_error(req);
        }
        char value[1000];
        snprintf(value, sizeof(value), "inline; filename=\"%s\"", &slashPos[1]);
        res->add_header(res, "Content-Disposition", value);
    }
    res->add_header(res, "Content-Type", ct->content_type);
    res->status_code = 200;
    return res;
}
Response* get_calc_handler(Request* req)
{
    UNUSED(req);
    Response* res = response_create();
    res->set_body(res, "Calculation goes here\n");
    return res;
}
Response* bad_request_handler(Request* req)
{
    UNUSED(req);
    Response* res = response_create();
    // yeah, this couples the code to the struct design. Should be fixed, but
    // this is ok since it's the only place that does this
    res->set_body(res, "Bad request\n");
    res->status_code = 400;
    return res;
}
Response* no_resource_handler(Request* req)
{
    Response* res = response_create();
    // yeah, this couples the code to the struct design. Should be fixed, but
    // this is ok since it's the only place that does this
    asprintf(&res->body, "Resource not found: %s\n", req->path);
    res->body_length = strlen(res->body);
    res->status_code = 404;
    return res;
}
Response* internal_error(Request* req)
{
    Response* res = response_create();
    // yeah, this couples the code to the struct design. Should be fixed, but
    // this is ok since it's the only place that does this
    asprintf(&res->body, "Resource not found: %s\n", req->path);
    res->body_length = strlen(res->body);
    res->status_code = 404;
    return res;
}
