#ifndef VEC_H
#define VEC_H

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    void **array;
    size_t size;
    size_t max_size;
    cmp_t cmp;
} vec_t;

typedef int (*cmp_t)(void*, void*);

int cmp(void *a, void *b);

vec_t create_vec(size_t max_size, cmp_t cmp);
vec_t place_vec(void *addr, size_t max_size, cmp_t cmp);
void destroy_vec(vec_t *vec);
void insert_vec(void *item, vec_t *vec);
void *lookup_vec(size_t i, vec_t *vec);
void remove_vec(size_t i, vec_t *vec);

#endif
