#ifndef INCLUDED_ALLOCATOR_UTILS_H
#define INCLUDED_ALLOCATOR_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define word_t intptr_t

#ifdef __cplusplus
extern "C" {
#endif
const size_t word_s = sizeof(word_t);

struct Block{
    size_t size;
    bool used;
    struct Block* next;
    /* 
    basic payload of the block
    but not necessarily sizeof(data[1]) is the whole memory
    block represent, size represents the size of the block
    */
    word_t data[1];
};

struct BlockHeader{
    size_t size;
    bool used;
    struct Block* next;
};

size_t align(size_t);

word_t *alloc(size_t);

size_t alloc_size(size_t);

struct Block *request_from_os(size_t);

struct Block *get_block_header(word_t*);

struct Block *find_free_block(size_t);

void split_block(struct Block*, size_t);

void coalesce_block(struct Block *);

void reset();

#ifdef __cplusplus
}
#endif

#endif