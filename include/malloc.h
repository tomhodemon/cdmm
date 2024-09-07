#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h> // size_t

#define NFASTBINS 10

typedef struct malloc_chunk
{
    size_t size;
    size_t prev_size;
    
    int prev_inuse;

    struct malloc_chunk *fd;
    struct malloc_chunk *bk;

} malloc_chunk;

typedef struct malloc_state
{
    malloc_chunk *top_chunk;

    malloc_chunk *fastbins[NFASTBINS];

    size_t system_mem;

} malloc_state;

extern void *malloc(size_t size);
extern void free(void *p);

#endif // MALLOC_H