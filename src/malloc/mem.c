#include "mem.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <unistd.h>


struct mem* g_mem_chunk_head =  NULL;

static void* allocate_chunk(struct mem* const current_chunk, struct mem* const previous_chunk, size_t query);

static void assign_to_chunk(struct mem* chunk, void* next, size_t capacity, bool is_free);

static void* try_mmap(size_t size, void* const addr);

static void* heap_init(size_t initial_size);

void* _malloc( size_t query ){
    if(g_mem_chunk_head == NULL){
        if(heap_init(getpagesize()) == NULL){
            return NULL;
        }
    }

    if(query < BLOCK_MIN_SIZE){
        query = BLOCK_MIN_SIZE;
    }

    struct mem* current_chunk = g_mem_chunk_head;
    struct mem* previous_chunk = current_chunk;
/*
    iterate until the current chunk is NULL or the current chunk
    is free and has enough space to be spit into a smaller chunk
 */
    while(current_chunk != NULL &&
            (!current_chunk->is_free
          || current_chunk->capacity < query + sizeof(struct mem) + BLOCK_MIN_SIZE)){
        previous_chunk = current_chunk;
        current_chunk = current_chunk->next;
    }
    return allocate_chunk(current_chunk, previous_chunk, query);
}

void  _free( void* mem ){
    struct mem* chunk = (void *)((uint8_t *)mem - sizeof(struct mem));
    //merge chunks if contiguous and free
    if(chunk->next->is_free && chunk->next == (void *)((uint8_t *)mem + chunk->capacity)){
        assign_to_chunk(chunk, chunk->next->next, chunk->capacity + chunk->next->capacity + sizeof(mem), true);
    }
    chunk->is_free = true;
}


// BLOCK_MIN_SIZE is added to ensure that when a chunck is split,
// each split chunk will be at least BLOCK_MIN_SIZE
static void* allocate_chunk(struct mem* current_chunk, struct mem* const previous_chunk, size_t query){
    if(current_chunk == NULL){
        size_t pagesize = getpagesize();
        size_t allocate_size;
        if(query + sizeof(struct mem) * 2 + BLOCK_MIN_SIZE  < pagesize){
            allocate_size = pagesize;
        } else{
            size_t block_size = query + sizeof(struct mem) * 2 + BLOCK_MIN_SIZE;
            allocate_size = pagesize * (block_size % pagesize ? block_size / pagesize + 1 : block_size / pagesize);
        }
        struct mem* new_chunk = try_mmap(allocate_size,
                (uint8_t *)previous_chunk + previous_chunk->capacity + sizeof(struct mem));
        if(new_chunk == NULL){
            return NULL;
        }
        assign_to_chunk(new_chunk, NULL, allocate_size - sizeof(struct mem), true);
        previous_chunk->next = new_chunk;
        current_chunk = new_chunk;

    }
    /*split chunk in 2 we know there will be space to split in 2 due to when this function is called (see _malloc)*/
    struct mem* next_chunk = (void *)((uint8_t *)current_chunk + sizeof(struct mem) + query);
    size_t remaining_capacity = current_chunk->capacity - sizeof(struct mem) - query;
    assign_to_chunk(next_chunk, current_chunk->next, remaining_capacity, true);
    assign_to_chunk(current_chunk, next_chunk, query, false);
    return (uint8_t *)current_chunk + sizeof(struct mem);
}


static void* heap_init( size_t initial_size ){
    g_mem_chunk_head = try_mmap(initial_size, HEAP_START);
    assign_to_chunk(g_mem_chunk_head, NULL, initial_size - sizeof(struct mem), true);
    return g_mem_chunk_head;
}




static void* try_mmap(size_t size, void* const addr){
    struct mem* new_chunk = mmap(addr, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0);
    if(new_chunk == ((void*)-1)){
        new_chunk = mmap(addr, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if(new_chunk == ((void*)-1)){
            new_chunk = NULL;
        }
    }
    return new_chunk;
}


static void assign_to_chunk(struct mem* const chunk, void* const next, size_t capacity, bool is_free){
    chunk->next = next;
    chunk->capacity = capacity;
    chunk->is_free = is_free;
}
