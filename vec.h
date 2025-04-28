#ifndef VEC_H
#define VEC_H

#include <stdlib.h> // For malloc, realloc, free

typedef struct {
    void** data;      // Array of void pointers (can store any type)
    size_t size;      // Current number of elements
    size_t capacity;  // Maximum number of elements
} Vec;

Vec* vec_new(size_t initial_capacity);
void vec_free(Vec* vec);
void vec_push(Vec* vec, void* element);
void* vec_get(Vec* vec, size_t index);
size_t vec_size(Vec* vec);

#endif // VEC_H
