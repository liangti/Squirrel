#include <memory/allocator.h>
#include <memory/block.h>

namespace sql{

namespace {
static Block* _head = _block_head;
static Block* _top = _block_top;
static size_t _allocated = _block_allocated;

}; // anonymous namespace

class AllocatorImpl{
private:
    Block* find_free_block(size_t size){
        auto block = _head;

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
    AllocatorImpl(){}
    word_t* allocate(size_t size) {
        size = align(size);
        if(auto block = find_free_block(size)){
            block->used = true;
            _allocated += size;
            return block->data;
        }
        auto block = request_from_os(size);
        block->next = nullptr;
        if (_head == nullptr){
            _head = block;
        }
        // Chain the blocks
        if (_top != nullptr){
            _top->next = block;
        }
        _top = block;
        _allocated += block->size;

        return block->data;
    }
    void deallocate(word_t* p) {
        Block* block = get_block_header(p);
        if(block->used){
            _allocated -= block->size;
        }
        block->used = false;
    }
    void clear(){
        Block* ptr = _head;
        while(ptr != nullptr){
            deallocate(ptr->data); 
            ptr = ptr->next;
        }
        _head = nullptr;
        _top = nullptr;
        // TODO: sbrk negative
    }
    size_t allocated_size(){
        return _allocated;
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