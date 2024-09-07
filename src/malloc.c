/*
    Supported pointer representation:       8 bytes
    Supported size_t representation:        8 bytes
*/

#include "malloc.h"
#include "utils.h"
#include "debug.h"

#include <unistd.h> // sbrk()
#include <stdio.h> // printf()

int _malloc_initialized = 0;
static malloc_state mstate;
#define INITIAL_HEAP_SIZE 1000

/* Size of a malloc_chunk header */
#define SIZE_HEADER sizeof(malloc_chunk)

/* Number of fast bins */
#define NFASTBINS   10

/* Maximum chunk size that can be handled by fast bins */
#define BINS_MAX_SIZE   128

/* Set chunk size */
#define set_head_size(p, s)     ((p)->size = (s))

/* Treat space at ptr + offset as a chunk */
#define chunk_at_offset(p, s)   ((malloc_chunk*) (((char *) (p)) + (s)))

/* Get size */
#define chunksize(p)    ((p)->size)

/* Conversion from malloc headers to user pointers, and back */
#define chunk2mem(p)   ((void*)((char*)(p) + SIZE_HEADER))
#define mem2chunk(mem) ((malloc_chunk*)((char*)(mem) - SIZE_HEADER))

/* Round the size up to the nearest multiple of 8 and returns bin index*/
#define FASTBIN_IDX(x)  ((x+7) >> 3) - 1

#define _PRINT_SIZE(s)  printf("%s: %zu\n", #s, s)
#define _PRINT_ADDR(p)  printf("%s addr: %p\n", #p, p)

void _init_malloc(void);
void free (void *mem);
void _debug_malloc_chunk_visualizer(malloc_chunk *chunk);
void _debug_malloc_state_visualizer(void);

void
_init_malloc(void)
{
    void *top_chunk = NULL;

    top_chunk = sbrk(INITIAL_HEAP_SIZE);
    if (top_chunk == (void*)-1)
        _exit(EXIT_FAILURE);

    mstate.top_chunk = (malloc_chunk*) top_chunk;
    mstate.top_chunk->size = INITIAL_HEAP_SIZE - SIZE_HEADER;
    mstate.top_chunk->bk = NULL;
    mstate.top_chunk->fd = NULL;

    mstate.system_mem = INITIAL_HEAP_SIZE;

    for (int i = 0; i < NFASTBINS; i++)
    {
        mstate.fastbins[i] = NULL;
    }

    _malloc_initialized = 1;
    return; 
}


void *
malloc(size_t bytes)
{   
    printf("\n[%s]\n", __func__);

    size_t nb;                  /* normalized request size */
    unsigned int idx;           /* associated bin index */
    malloc_chunk **fb;          /* associated fastbin */

    malloc_chunk *victim;       /* inspected/selected chunk */
    size_t size;                /* its size */

    malloc_chunk *remainder;    /* remainder from a split */
    size_t remainder_size;      /* its size */

    void* p;                    /* pointer to the newly allocated memory space */

    if (!_malloc_initialized)
    {
        _init_malloc();
    }

    /* 
        Normalize request size. 
        Convert request size to obtain necessary size of at least 
        MINSIZE, the smallest allocatable size.
    */
    nb = bytes + SIZE_HEADER;
    printf("bytes (requested size): %zu\n", bytes);
    printf("nb: %zu\n", nb);

    if (nb <= BINS_MAX_SIZE)
    {
        // fastbins insertion
    
        idx = FASTBIN_IDX(nb);

#ifdef DEBUG
        printf("idx: %d\n", idx);
#endif

        /*
            Find a suitable chunk
        */
        fb = &(mstate.fastbins[idx]);

        _PRINT_ADDR(*fb);
        
        if (!(*fb))
        {   
            printf("bin is NULL\n");
            // no suitable chunk find in fastbins, allocate memory from the top_chunk
            // trim a chunk for the requested allocation 
            // adjust the size of the top_chunk accordingly

            // check if top_chunk is large enough

            victim = mstate.top_chunk;
            size = victim->size;

            if (size >= nb)
            {   
                remainder_size =  size - nb;
                remainder = chunk_at_offset(victim, nb);
                mstate.top_chunk = remainder;
        
                set_head_size(remainder, remainder_size);
                set_head_size(victim, nb);

                p = chunk2mem(victim);

            }
            else
            {
                printf("top_chunk is not large enough\n");
                _exit(EXIT_FAILURE);
            }
        } 
        else
        {
            victim = *fb;
            *fb = victim->bk;
            p = chunk2mem(victim);
        }   
    }
   
    printf("[%s]\n\n", __func__);
    return p;
}   


void
free (void *mem)
{   
    printf("\n[%s]\n", __func__);

    malloc_chunk *p;        /* inspected/selected chunk */
    unsigned int idx;       /* its possible eligible fast bin index */
    size_t size;            /* its size */
    malloc_chunk **fb;      /* associated fastbin */


    p = mem2chunk(mem);
    _PRINT_ADDR(p);

    size = chunksize(p);
    _PRINT_SIZE(size);

    /*
        If eligible, place chunk on a fastbin so it can be found
        and used quickly in malloc.
    */
    if (size <= BINS_MAX_SIZE)
    {  
        idx = FASTBIN_IDX(size);
        fb = &(mstate.fastbins[idx]);

        /*
            Add free chunk at the front position of the fast bin.
        */
        p->bk = *fb;
        *fb = p;     
    }
   
    printf("[%s]\n\n", __func__);
    return;
}

// DEBUG
void 
_debug_malloc_state_visualizer(void)
{
    printf("[%s]\n\t", __func__);

    printf("addr: %p" 
        " | top_chunk: %p"
        " | system_mem: %zu",
        mstate,
        mstate.top_chunk,
        mstate.system_mem);

    long diff;
    diff = sbrk(0) - (void*) mstate.top_chunk;
    printf("\ndiff: %ld", diff);

    printf("\n[%s]\n\n", __func__);
}

void 
_debug_malloc_chunk_visualizer(malloc_chunk *chunk)
{   
    printf("\n[%s]\n\t", __func__);
    
    printf("addr: %p" 
        " | size: %zu" 
        " | prev_size: %zu" 
        " | fd: %p" 
        " | bk: %p\n", 
        chunk, 
        chunk->size, 
        chunk->prev_size, 
        chunk->fd, 
        chunk->bk);

    printf("[%s]\n\n", __func__);
}