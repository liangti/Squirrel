#include <memory/utility.h>

namespace sql{

extern "C"{
    Block* _block_head = nullptr;
    Block* _block_top = nullptr;
    size_t _block_allocated = 0;
};
static std::map<BlockData*, BlockHeader*> _block_map;
static size_t _memory_size = 0;

void BlockManager::add_block(BlockData* data, BlockHeader* header){
    auto temp = _block_map.find(data);
    if(temp == _block_map.cend()){
        auto block = (Block*)header;
        _block_map.insert(std::make_pair(data, header));
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
    _memory_size += header->size;
}

bool BlockManager::valid_block(BlockData* data){
    return _block_map.find(data) != _block_map.cend();
}

BlockHeader* BlockManager::get_block_header(BlockData* data){
    auto ptr = _block_map.find(data);
    if(ptr == _block_map.cend()){
        return nullptr;
    }
    return ptr->second;
}

void BlockManager::free_block(BlockData* data){
    auto temp = _block_map.find(data);
    if(temp != _block_map.cend()){
        BlockHeader* header = temp->second;
        if(header->used){
            _memory_size -= header->size;
        }
        header->used = false;
    }
}

void BlockManager::split_block(Block* block, size_t size){
    split(block, size);
    // insert next block into map
    Block* next_block = block->next;
    _block_map.insert(std::make_pair((BlockData*)next_block->data, (BlockHeader*)next_block));
    _memory_size += block->size;
}

Block* BlockManager::get_head(){
    return _block_head;
}

Block* BlockManager::get_top(){
    return _block_top;
}

void BlockManager::clear(){
    for(auto itr = _block_map.begin(); itr != _block_map.end(); itr++){
        itr->second->used = false;
    }
    _memory_size = 0;
}

size_t BlockViewer::num(){
    return _block_map.size();
}

size_t BlockViewer::size(){
    return _memory_size;
}

}; // namespace sql;