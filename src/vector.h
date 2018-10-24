#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include <stddef.h>

// Assume that we create vector<T>
struct vector {
    // T[]
    void* ptr;
    // actual size in memory, should not be used by application
    size_t size;
    // number of elements.
    int length;
    size_t element_size;
};

// vector<T>* vector_new(size_t, element_size)
struct vector* vector_new(size_t element_size);
// void vector_delete(vector<T>*)
void vector_delete(struct vector*);
// T* vector_at(vector<T>*, int)
void* vector_at(struct vector*, int);
// T* vector_put(vector<T>*, int, T*)
void* vector_put(struct vector*, int, void*);
// int vector_length(vector<T>*)
int vector_length(struct vector*);
// T* vector_push_back(vector<T>, T*)
void* vector_push_back(struct vector*, void*);

#endif
