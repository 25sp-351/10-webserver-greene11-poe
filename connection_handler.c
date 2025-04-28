#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "connection_handler.h"
#include "request.h"
#include "response.h"
#include "router.h"

// threaded; handles multiple messages; closes connection when done

// input pointer is freed by this function
void handleConnection(int* sock_fd_ptr)
{
    int sock_fd = *sock_fd_ptr;
    free(sock_fd_ptr);

    printf("Handling connection on %d\n", sock_fd);
    while (1) {
        Request* req = request_read_from_fd(sock_fd);
        if (req == NULL) {
            break;
        }
        //request_print(req);

        Response* response = route_request(req);
        req->free(req);

        //response_print(response);
        if (response == NULL)
            break;

        bool result = response->send_to_fd(response, sock_fd);
        response->free(response);
        if (!result) {
            break;
        }
    }
    printf("done with connection %d\n", sock_fd);
    close(sock_fd);
}

    