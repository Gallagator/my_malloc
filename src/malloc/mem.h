#ifndef _MEM_H_
#define _MEM_H_
#define _USE_MISC

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <unistd.h>

#define HEAP_START  ((void*)0x04040000)
#define BLOCK_MIN_SIZE 16LU


struct mem;

extern struct mem* g_mem_chunk_head;

#pragma pack(push, 1)
struct mem  {
    struct mem* next;
    size_t capacity;
    bool is_free;
};
#pragma pack(pop)

void* _malloc( size_t query );

void  _free( void* mem );

#endif
