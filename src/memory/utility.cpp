#include <memory/utility.h>

namespace sql{

extern "C"{
    Block* _block_head = nullptr;
    Block* _block_top = nullptr;
};
static size_t _memory_size = 0;

// mark block used and add to chain if the block is not there yet.
void BlockManager::add_block(BlockData* data, BlockHeader* header, bool is_new){
    auto block = (Block*)header;
    if (is_new){
        block->next = nullptr;
        if (_block_head == nullptr){
            _block_head = block;
        }
        // Chain the blocks
        if (_block_top != nullptr){
            _block_top->next = block;
        }
        _block_top = block;
    }
    _memory_size += size_get((Block*)header);
    used_set((Block*)header);
}

void BlockManager::free_block(BlockData* data){
    BlockHeader* header = (BlockHeader*)get_header(data);
    if(used((Block*)header)){
        _memory_size -= size_get((Block*)header);
    }
    used_clear((Block*)header);
}

void BlockManager::split_block(Block* block, size_t size){
    split(block, size);
    // edit memory size
}

Block* BlockManager::get_head(){
    return _block_head;
}

Block* BlockManager::get_top(){
    return _block_top;
}

void BlockManager::clear(){
    for(auto itr = _block_head; itr != _block_top; itr++){
        used_clear(itr);
    }
    _memory_size = 0;
}

size_t BlockViewer::size(){
    return _memory_size;
}

}; // namespace sql;