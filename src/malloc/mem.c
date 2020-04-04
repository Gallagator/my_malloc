#include "mem.h"

struct mem* g_mem_chunk_head =  NULL;

/*mmap(HEAP_START, len, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_FIXED, -1, 0);*/


void* _malloc( size_t query ){
    if(g_mem_chunk_head == NULL){
        if(heap_init(INITIAL_HEAP_SIZE) == NULL){
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

static void* allocate_chunk(struct mem* const current_chunk, struct mem* const previous_chunk, size_t query){
    if(current_chunk == NULL){
        struct mem* new_chunk = try_mmap(query + sizeof(struct mem),
                                         (void*)previous_chunk + previous_chunk->capacity + sizeof(struct mem));
        if(new_chunk == NULL){
            return NULL;
        }
        assign_to_chunk(new_chunk, NULL, query, false);
        previous_chunk->next = new_chunk;
        return (void*)new_chunk + sizeof(struct mem);
    }
    /*split chunk in 2 we know there will be space to split in 2 due to when this function is called (see _malloc)*/
    struct mem* next_chunk = (void*)current_chunk + sizeof(struct mem) + query;
    size_t remaining_capacity = current_chunk->capacity - sizeof(struct mem) * 2 - query;
    assign_to_chunk(next_chunk, current_chunk->next, remaining_capacity, true);
    assign_to_chunk(current_chunk, next_chunk, query, false);
    return (void*)current_chunk + sizeof(struct mem);
}


void* heap_init( size_t initial_size ){
    g_mem_chunk_head = try_mmap(initial_size, HEAP_START);
    assign_to_chunk(g_mem_chunk_head, NULL, initial_size, true);
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
