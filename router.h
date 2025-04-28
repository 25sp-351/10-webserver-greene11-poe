#ifndef ROUTER_H
#define ROUTER_H

#include "request.h"
#include "response.h"
#include <stdbool.h>

// Forward declarations of handler functions
typedef struct Response Response;
typedef struct Request Request;

Response* get_stats_handler(Request* req);
Response* get_static_handler(Request* req);
Response* get_calc_handler(Request* req);
Response* no_resource_handler(Request* req);
Response* internal_error(Request* req);
Response* bad_request_handler(Request* req);

// Structure for defining routes
typedef struct {
    char* method;
    char* path;
    bool full_match;
    Response* (*handler)(Request*);
} Route;

// Function to route a request to the appropriate handler
Response* route_request(Request* req);

// Structure for defining content types
typedef struct {
    char* extension;
    char* content_type;
    bool as_file;
} ContentType;

// Function to determine the content type for a given file path
ContentType* content_type_for_file(char* path);

// Function to check if a string ends with a specific suffix
char *strendswith(const char *str, const char *suffix);

#endif // ROUTER_H