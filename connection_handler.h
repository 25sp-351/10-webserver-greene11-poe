#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// Forward declarations for structures defined in other headers
typedef struct Request Request;
typedef struct Response Response;

// Function to handle a single client connection
void handleConnection(int* sock_fd_ptr);

#endif // CONNECTION_HANDLER_H