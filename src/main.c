#include <stdio.h>

#include "malloc/mem.h"
#include "malloc/mem_dbg.h"

int main() {
 //   int* k = (int*)_malloc(sizeof(int) *   100);
    int* j = (int*)_malloc(sizeof(int) * 20);
    uint64_t* l = (uint64_t*)_malloc(sizeof(uint64_t) * 20);


//    printf("%d %d %d %d\n", *k, *j, *l, *m);
    printf("%p %p\n", j, l);

    memalloc_debug_heap(stdout, g_mem_chunk_head);

    return 0;
}
