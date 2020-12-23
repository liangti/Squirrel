#include <memory/allocator.h>
#include <unistd.h>
#include <utility>

namespace sql{

namespace {

Block *heap_start = nullptr;
Block *heap_top = heap_start;

// heap begin address
char* heap_begin = (char *)sbrk(0);
}; // anonymous namespace

// reset heap start position
void reset(){
    heap_start = nullptr;
    heap_top = heap_start;
}

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
    return block;
}

// Memory Allocate
word_t *alloc(size_t size){
    size = align(size);

    if(auto block = find_free_block(size)){
        block->used = true;
        return block->data;
    }

    auto block = request_from_os(size);
    block->size = size;
    block->used = true;

    if (heap_start == nullptr){
        heap_start = block;
    }

    // Chain the blocks
    if (heap_top != nullptr){
        heap_top->next = block;
    }

    heap_top = block;

    return block->data;
}

Block *get_block_header(word_t *data){
    return (Block *)((char *)data + sizeof(std::declval<Block>().data) - sizeof(Block));
}

// Memory Release
void free(word_t *data){
    auto block = get_block_header(data);
    block->used = false;
}

Block* find_free_block(size_t size){
    auto block = heap_start;

    while(block != nullptr){
        if (block->used || block->size < size){
            block = block->next;
            continue;
        }
        return block;
    }
    return nullptr;
}

Block* split_block(Block *block, size_t size){
    // assume size here is already align
    if(block->size <= size){
        return nullptr;
    }
    
    Block *new_block = (Block *)((char*)(block) + size);
    new_block->size = block->size - size;
    block->size = size;

    // add new block into the chain
    Block *previous_next = block->next;
    block->next = new_block;
    new_block->next = previous_next;
    return block;
}

// only walk one step
Block* coalesce_block(Block *block){
    if(block == nullptr){
        return block;
    }
    Block *walk_ptr = block->next;
    if(walk_ptr != nullptr){
        block->size += walk_ptr->size;
        block->next = walk_ptr->next;
    }
    return block;
}

size_t memory_size(){
    return (char *)sbrk(0) - heap_begin;
}

}; // namespace sql