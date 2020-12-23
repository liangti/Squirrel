#ifndef INCLUDED_MEMORY_ALLOCATOR_H
#define INCLUDED_MEMORY_ALLOCATOR_H
#include <stdint.h>
#include <cstddef>

namespace sql{



using word_t = intptr_t;

const size_t word_s = sizeof(word_t);

struct Block{
    size_t size;

    bool used;

    Block *next;
    
    /* 
    basic payload of the block
    but not necessarily sizeof(data[1]) is the whole memory
    block represent, size represents the size of the block
    */
    word_t data[1];
};

size_t align(size_t);

word_t *alloc(size_t);

size_t alloc_size(size_t);

Block *request_from_os(size_t);

Block *get_block_header(word_t*);

void free(word_t*);

Block *find_free_block(size_t);

Block *split_block(Block*,size_t);

Block* coalesce_block(Block *);

void reset();

size_t memory_size();

}; // namespace sql

#endif