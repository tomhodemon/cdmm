This is an oversimplified version of malloc for educational purposes. This implementation disregards data structure alignment and threading. Fast bin is currently the only bin supported.

## Contents 
**Standard functions:**  
- malloc(size_t n)  
- free(void* p)

## Quickstart

````
make && ./bin/main 
````

## Ressources
  - A Memory Allocator by Doug Lea: https://gee.cs.oswego.edu/dl/html/malloc.html

  - Understanding-glibc-malloc by sploitfun: https://sploitfun.wordpress.com/2015/02/10/understanding-glibc-malloc/