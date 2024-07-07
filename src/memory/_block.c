// system headers
#include <unistd.h>
#include <sys/mman.h>


#include <memory/_block.h>

bool used(block_t *block) { return block->_size & 1; }

void used_set(block_t *block) { block->_size |= 1; }

void used_clear(block_t *block) { block->_size &= ~1; }

size_t get_size(block_t *block) { return block->_size ^ (block->_size & 1); }

void set_size(block_t *block, size_t size) { block->_size = size; }

/*
Memory Align:
- x64
align(3) -> 8
algin(12) -> 16
- x32
align(3) -> 4
align(12) -> 12
*/
size_t align(size_t n) { return (n + word_s - 1) & ~(word_s - 1); }

size_t alloc_size(size_t size) { return align(size + sizeof(block_header_t)); }

block_t *request_block_from_os(size_t size) {
  // current top of the heap
  // if sbrk success, it will also be the beginning of the new block
  block_t *block = (block_t *)sbrk(0);
    if (sbrk(alloc_size(size)) == (void *)-1) {
      return NULL;
    }
  set_size(block, alloc_size(size));
  used_set(block);
  return block;
}

block_t *request_large_block_from_os(size_t size){
  block_t *block = mmap(0, alloc_size(size), PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
  if (block == NULL){
    return block;
  }
  set_size(block, alloc_size(size));
  used_set(block);
  return block;
}

block_header_t *get_header(word_t *data) {
  return (block_header_t *)data - 1;
}

block_t *get_block(word_t *data){
  return (block_t*)((block_header_t *)data - 1);
}

// Mark block as unused
void release_block(block_t *block) {
  used_clear(block);
}

// return memory back to os
int release_large_block_to_os(block_t *block) {
  return munmap(block, block->_size);
}

void split(block_t *block, size_t size) {
  if (used(block)) {
    return;
  }

  // assume size here is already align
  if (get_size(block) <= size) {
    return;
  }

  block_t *new_block =
      (block_t *)((char *)(block) + size + sizeof(block_header_t));
  set_size(new_block, get_size(block) - size - sizeof(block_header_t));
  set_size(block, size);

  // add new block into the chain
  block_t *previous_next = block->next;
  block->next = new_block;
  new_block->next = previous_next;
}

// only walk one step
void coalesce_block(block_t *block) {
  if (block == NULL) {
    return;
  }
  if (used(block)) {
    return;
  }
  block_t *walk_ptr = block->next;
  if (walk_ptr != NULL && !used(walk_ptr)) {
    set_size(block, get_size(block) + get_size(walk_ptr));
    block->next = walk_ptr->next;
  }
  return;
}