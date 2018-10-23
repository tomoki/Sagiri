#pragma once

#include <stddef.h>

struct vector {
    void* ptr;
    // actual size in memory, should not be used by application
    size_t size;
    // number of elements.
    int length;
    size_t element_size;
};

struct vector* vector_new(size_t element_size);
void vector_delete(struct vector*);
void* vector_at(struct vector*, int);
void* vector_put(struct vector*, int, void*);
int vector_length(struct vector*);
void* vector_push_back(struct vector*, void*);
