#include <memory/allocator.h>
#include <memory/block.h>

namespace sql{

class AllocatorImpl{
private:
    Block* head;
    Block* top;
    size_t allocated;
    Block* find_free_block(size_t size){
        auto block = head;

        while(block != nullptr){
            if (block->used || block->size < size){
                block = block->next;
                continue;
            }
            return block;
        }
        return nullptr;
    }
public:
    AllocatorImpl():head(nullptr), top(nullptr), allocated(0){}
    word_t* allocate(size_t size) {
        size = align(size);
        if(auto block = find_free_block(size)){
            block->used = true;
            allocated += size;
            return block->data;
        }
        auto block = request_from_os(size);
        block->next = nullptr;
        if (head == nullptr){
            head = block;
        }
        // Chain the blocks
        if (top != nullptr){
            top->next = block;
        }
        top = block;
        allocated += block->size;

        return block->data;
    }
    void deallocate(word_t* p) {
        Block* block = get_block_header(p);
        if(block->used){
            allocated -= block->size;
        }
        block->used = false;
    }
    void clear(){
        Block* ptr = head;
        while(ptr != nullptr){
            deallocate(ptr->data); 
            ptr = ptr->next;
        }
        head = nullptr;
        top = nullptr;
        // TODO: sbrk negative
    }
    size_t allocated_size(){
        return allocated;
    }
    ~AllocatorImpl(){
        clear();
    }
};

AllocatorBase::AllocatorBase(){
    impl = new AllocatorImpl();
}

word_t* AllocatorBase::allocate(size_t size){
    return impl->allocate(size);
}

void AllocatorBase::deallocate(word_t* p){
    impl->deallocate(p);
}

void AllocatorBase::clear(){
    impl->clear();
}

size_t AllocatorBase::allocated_size(){
    return impl->allocated_size();
}

}; // namespace sql