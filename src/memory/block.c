#include <unistd.h>
#include <memory/block.h>


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
    return size + sizeof(block_t);
}

block_t* request_from_os(size_t size){
    // current top of the heap
    // if sbrk success, it will also be the beginning of the new block
    block_t* block = (block_t *)sbrk(0);
    if (sbrk(alloc_size(size)) == (void *)-1){
        return NULL;
    }
    block->size = size;
    block->used = true;
    return block;
}



block_t* get_header(word_t *data){
    return (block_t*)((char *)data - sizeof(block_header_t));
}

// Mark block as unused
void free(word_t *data){
    block_t* block = get_header(data);
    block->used = false;
}

void split(block_t* block, size_t size){
    if(block->used){
        return;
    }
    
    // assume size here is already align
    if(block->size <= size){
        return;
    }
    
    block_t *new_block = (block_t *)((char*)(block) + size + sizeof(block_header_t));
    new_block->size = block->size - size - sizeof(block_header_t);
    block->size = size;

    // add new block into the chain
    block_t *previous_next = block->next;
    block->next = new_block;
    new_block->next = previous_next;
}

// only walk one step
void coalesce_block(block_t *block){
    if(block == NULL){
        return;
    }
    if(block->used){
        return;
    }
    block_t *walk_ptr = block->next;
    if(walk_ptr != NULL && !walk_ptr->used){
        block->size += walk_ptr->size;
        block->next = walk_ptr->next;
    }
    return;
}