#include <stdio.h>

void
diff(void *a, void *b)
{
    long diff;
    diff = a - b;
    printf("diff: %ld\n", diff);
};