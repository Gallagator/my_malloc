#include <stdio.h>

#include "malloc/mem.h"
#include "malloc/mem_dbg.h"

int main() {
 //   int* k = (int*)_malloc(sizeof(int) *   100);

    int* j = _malloc(sizeof(int)*32);
    for (int i = 0; i <32 ; ++i) {
        j[i] = i;
    }
    int* k = _malloc(sizeof(int)*200);
    for (int i = 0; i <200 ; ++i) {
        k[i] = i;
    }
    int* l = _malloc(sizeof(int)*3000);
    for (int i = 0; i <3000 ; ++i) {
        l[i] = i;
    }
    _free(l);
    _free(k);
    _free(j);

    memalloc_debug_heap(stdout, g_mem_chunk_head);

    return 0;
}
