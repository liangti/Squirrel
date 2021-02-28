#include <memory/utility.h>
#include <memory/allocator.h>

namespace sql{

class AllocatorImpl{
private:
    BlockManager manager;
    BlockAgent find_free_block(size_t size){
        auto block = manager.get_head();

        while(!block.null()){
            if (block.is_used() || block.size() < size || block.size() >= size * 2){
                block.next();
                continue;
            }
            // TODO split the large block
            // maybe in Reclaimer
            return block;
        }
        return BlockAgent();
    }
public:
    AllocatorImpl(){}
    word_t* allocate(size_t size) {
        bool is_new = false;
        size = align(size);
        BlockAgent block = find_free_block(size);
        if(block.null()){
            block = manager.get_new_block(size);
            is_new = true;
        }
        manager.add_block(block, is_new);
        return block.get_data();
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