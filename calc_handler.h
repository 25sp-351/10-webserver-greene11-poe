#ifndef CALC_HANDLER_H
#define CALC_HANDLER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Request Request;
typedef struct Response Response;

typedef enum {
    CALC_OK = 0,
    CALC_INVALID_OPERATION,
    CALC_INVALID_OPERAND,
    CALC_DIVISION_BY_ZERO,
    CALC_PATH_ERROR
} CalcStatus;

Response* get_calc_handler(Request* req);

#endif
