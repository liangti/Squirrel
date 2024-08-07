#ifndef INCLUDED_ALLOCATOR_UTILS_H
#define INCLUDED_ALLOCATOR_UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define word_t intptr_t
#define block_t struct Block
#define block_header_t struct BlockHeader

#ifdef __cplusplus
extern "C" {
#endif
const size_t word_s = sizeof(word_t);

struct Block {
  // due to the memory alignment, size cannot be 1
  // the first bit is used for marking block is using or not
  // directly visit _size outside of block is not recommended
  size_t _size;
  struct Block *next;
  /*
  basic payload of the block
  but not necessarily sizeof(data[1]) is the whole memory
  block represent, size represents the size of the block
  */
  word_t data[1];
};

struct BlockHeader {
  size_t _size;
  struct Block *next;
};

extern block_t *_block_head;
extern block_t *_block_top;
extern size_t _block_allocated;

size_t align(size_t);

size_t alloc_size(size_t);

block_t *request_block_from_os(size_t);

block_t *request_large_block_from_os(size_t);

void release_block(block_t *block);

// 0 for successful completion, -1 for fail
int release_large_block_to_os(block_t *block);

block_header_t *get_header(word_t *);

block_t *get_block(word_t *);

void split(block_t *, size_t);

void coalesce_block(block_t *);

void reset();

// bitwise for embedding used in size in Block struct
bool used(block_t *);

void used_clear(block_t *);

void used_set(block_t *);

size_t get_size(block_t *);

void set_size(block_t *, size_t);

#ifdef __cplusplus
}
#endif

#endif