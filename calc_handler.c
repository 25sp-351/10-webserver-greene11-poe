#include "calc_handler.h"
#include "response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#define BUFFER_SIZE 128

Response* get_calc_handler(Request* req) {
    if (req == NULL) {
        Response* res = response_create();
        res->status_code = 500; // Internal Server Error
        res->set_body(res, "Internal server error: Request is NULL\n");
        return res;
    }

    char* path = req->path;

    if (strncmp(path, "/calc/", 6) != 0) {
        Response* res = response_create();
        res->status_code = 404; // Not Found
        res->set_body(res, "Resource not found\n");
        return res;
    }

    path += 6;

    char* operation = strtok(path, "/");
    char* num1_str = strtok(NULL, "/");
    char* num2_str = strtok(NULL, "/");

    if (operation == NULL || num1_str == NULL || num2_str == NULL) {
        Response* res = response_create();
        res->status_code = 400;
        res->set_body(res, "Malformed request: Missing operation or operands\n");
        return res;
    }

    char* endptr;
    errno = 0;
    long num1_long = strtol(num1_str, &endptr, 10);
    if (errno != 0 || *endptr != '\0' || num1_long < INT_MIN || num1_long > INT_MAX) {
        Response* res = response_create();
        res->status_code = 400;
        res->set_body(res, "Invalid operand 1\n");
        return res;
    }
    int operand_1 = (int)num1_long;

    errno = 0;
    long num2_long = strtol(num2_str, &endptr, 10);
    if (errno != 0 || *endptr != '\0' || num2_long < INT_MIN || num2_long > INT_MAX) {
        Response* res = response_create();
        res->status_code = 400;
        res->set_body(res, "Invalid operand 2\n");
        return res;
    }
    int operand_2 = (int)num2_long;

    int result;
    if (strcmp(operation, "add") == 0) {
        result = operand_1 + operand_2;
    } else if (strcmp(operation, "multiply") == 0) {
        result = operand_1 * operand_2;
    } else if (strcmp(operation, "subtract") == 0) {
        result = operand_1 - operand_2;
    } else if (strcmp(operation, "divide") == 0) {
        if (operand_2 == 0) {
            Response* res = response_create();
            res->status_code = 400;
            res->set_body(res, "Division by zero\n");
            return res;
        }
        result = operand_1 / operand_2;
    } else {
        Response* res = response_create();
        res->status_code = 400;
        res->set_body(res, "Invalid operation\n");
        return res;
    }

    Response* res = response_create();
    char body[BUFFER_SIZE];
    snprintf(body, sizeof(body), "Result: %d\n", result);
    res->set_body(res, body);
    res->status_code = 200;
    return res;
}