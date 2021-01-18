#include <unistd.h>
#include <utility>
#include <memory/allocator_utils.h>


namespace internal{


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
    return size + sizeof(Block);
}

Block *request_from_os(size_t size){
    // current top of the heap
    // if sbrk success, it will also be the beginning of the new block
    auto block = (Block *)sbrk(0);
    if (sbrk(alloc_size(size)) == (void *)-1){
        return nullptr;
    }
    block->size = size;
    block->used = true;
    return block;
}



Block *get_block_header(word_t *data){
    return (Block *)((char *)data - sizeof(BlockHeader));
}

// Mark block as unused
void free(word_t *data){
    auto block = get_block_header(data);
    block->used = false;
}

void split_block(Block *block, size_t size){
    if(block->used){
        return;
    }
    
    // assume size here is already align
    if(block->size <= size){
        return;
    }
    
    Block *new_block = (Block *)((char*)(block) + size + sizeof(BlockHeader));
    new_block->size = block->size - size;
    block->size = size;

    // add new block into the chain
    Block *previous_next = block->next;
    block->next = new_block;
    new_block->next = previous_next;
}

// only walk one step
void coalesce_block(Block *block){
    if(block == nullptr){
        return;
    }
    if(block->used){
        return;
    }
    Block *walk_ptr = block->next;
    if(walk_ptr != nullptr && !walk_ptr->used){
        block->size += walk_ptr->size;
        block->next = walk_ptr->next;
    }
    return;
}

}; // namespace sql