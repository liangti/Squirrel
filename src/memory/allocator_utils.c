#include <unistd.h>
#include <memory/allocator_utils.h>


/*
Memory Align:
- x64
align(3) -> 8
algin(12) -> 16
- x32
align(3) -> 4
align(12) -> 12
*/
size_t align(size_t n){
    return (n + word_s - 1) & ~(word_s - 1);
}

size_t alloc_size(size_t size){
    return size + sizeof(struct Block);
}

struct Block* request_from_os(size_t size){
    // current top of the heap
    // if sbrk success, it will also be the beginning of the new block
    struct Block* block = (struct Block *)sbrk(0);
    if (sbrk(alloc_size(size)) == (void *)-1){
        return NULL;
    }
    block->size = size;
    block->used = true;
    return block;
}



struct Block* get_block_header(word_t *data){
    return (struct Block*)((char *)data - sizeof(struct BlockHeader));
}

// Mark block as unused
void free(word_t *data){
    struct Block* block = get_block_header(data);
    block->used = false;
}

void split_block(struct Block* block, size_t size){
    if(block->used){
        return;
    }
    
    // assume size here is already align
    if(block->size <= size){
        return;
    }
    
    struct Block *new_block = (struct Block *)((char*)(block) + size + sizeof(struct BlockHeader));
    new_block->size = block->size - size;
    block->size = size;

    // add new block into the chain
    struct Block *previous_next = block->next;
    block->next = new_block;
    new_block->next = previous_next;
}

// only walk one step
void coalesce_block(struct Block *block){
    if(block == NULL){
        return;
    }
    if(block->used){
        return;
    }
    struct Block *walk_ptr = block->next;
    if(walk_ptr != NULL && !walk_ptr->used){
        block->size += walk_ptr->size;
        block->next = walk_ptr->next;
    }
    return;
}