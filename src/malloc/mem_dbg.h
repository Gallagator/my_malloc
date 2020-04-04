//
// Created by liam on 04/04/2020.
//

#ifndef MY_MALLOC_MEM_DBG_H
#define MY_MALLOC_MEM_DBG_H

#include "mem.h"

#define DEBUG_FIRST_BYTES 4

void memalloc_debug_struct_info( FILE* f, struct mem const* const address );

void memalloc_debug_heap( FILE* f, struct mem  const* ptr );


#endif //MY_MALLOC_MEM_DBG_H
