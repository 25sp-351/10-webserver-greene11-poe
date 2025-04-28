#include "vec.h"
#include <stdio.h>
#include <string.h>

#define INITIAL_CAPACITY 8

Vec* vec_new(size_t initial_capacity) {
    Vec* vec = (Vec*)malloc(sizeof(Vec));
    if (vec == NULL) {
        perror("malloc");
        return NULL;
    }
    vec->size = 0;
    vec->capacity = initial_capacity;
    if (initial_capacity == 0) {
        initial_capacity = INITIAL_CAPACITY;
    }
    vec->data = (void**)malloc(sizeof(void*) * initial_capacity);
    if (vec->data == NULL) {
        perror("malloc");
        free(vec);
        return NULL;
    }
    return vec;
}

void vec_free(Vec* vec) {
    if (vec == NULL)
        return;
    free(vec->data);
    free(vec);
}

void vec_push(Vec* vec, void* element) {
    if (vec == NULL)
        return;

    if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        void** new_data = (void**)realloc(vec->data, sizeof(void*) * vec->capacity);
        if (new_data == NULL) {
            perror("realloc");
            return;
        }
        vec->data = new_data;
    }
    vec->data[vec->size++] = element;
}

void* vec_get(Vec* vec, size_t index) {
    if (vec == NULL || index >= vec->size)
        return NULL;
    return vec->data[index];
}

size_t vec_size(Vec* vec) {
    if (vec == NULL)
        return 0;
    return vec->size;
}
