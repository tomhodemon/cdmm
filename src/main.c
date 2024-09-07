#include "malloc.h"

#include <stdio.h>


int main()
{   
    void *ptr;
    ptr = malloc(10);  

    free(ptr); 

    ptr = malloc(10);  

    printf("ptr: %p\n", ptr);

    return 0;
}