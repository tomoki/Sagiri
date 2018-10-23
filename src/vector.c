#include "vector.h"

#include "util.h"

#include <string.h>
#include <stdlib.h>

#define VECTOR_INITIAL_LENGTH 10

void vector_expand(struct vector*);

struct vector* vector_new(size_t element_size)
{
    struct vector* ret = (struct vector*) malloc(sizeof(struct vector));
    size_t initial_size = element_size * VECTOR_INITIAL_LENGTH;
    ret->ptr = malloc(initial_size);
    ret->size = initial_size;
    ret->length = 0;
    ret->element_size = element_size;

    return ret;
}

void vector_delete(struct vector* v)
{
    free(v->ptr);
    free(v);
}

int vector_length(struct vector* v)
{
    return v->length;
}

void* vector_at(struct vector* v, int index)
{
    if (index >= v->length)
        error("Access to outside of vector");

    return v + v->element_size * index;
}

void* vector_put(struct vector* v, int index, void* p)
{
    void* loc = vector_at(v, index);
    memcpy(loc, p, v->element_size);
    return loc;
}

void* vector_push_back(struct vector* v, void* p)
{
    size_t required_size = (v->length + 1) * v->element_size;
    // vector_expand should be called at once because vector's size is always bigger than 1.
    // vector_expand will at least enough memory for another element.
    if (required_size > v->size)
        vector_expand(v);

    v->length++;
    return vector_put(v, v->length, p);
}

// private functions
void vector_expand(struct vector* v)
{
    size_t new_size = v->size * 2;
    v->ptr = realloc(v->ptr, new_size);
    v->size = new_size;
}
