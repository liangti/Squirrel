#include <memory/utility.h>
#include <memory/allocator.h>

namespace sql{

class AllocatorImpl{
private:
    BlockManager manager;
    Block* find_free_block(size_t size){
        auto block = manager.get_head();

        while(block != nullptr){
            if (block->used || block->size < size || block->size >= size * 2){
                block = block->next;
                continue;
            }
            // TODO split the large block
            // maybe in Reclaimer
            return block;
        }
        return nullptr;
    }
public:
    AllocatorImpl(){}
    word_t* allocate(size_t size) {
        size = align(size);
        Block* block = find_free_block(size);
        if(block == nullptr){
            block = request_from_os(size);
        }
        block->used = true;
        // add to BlockManager no matter what
        manager.add_block(block->data, (BlockHeader*)block);
        return block->data;
    }
    void deallocate(word_t* p) {
        manager.free_block(p);
    }
    ~AllocatorImpl(){
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


}; // namespace sql