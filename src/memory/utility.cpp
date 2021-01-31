#include <memory/utility.h>

namespace sql{

extern "C"{
    Block* _block_head = nullptr;
    Block* _block_top = nullptr;
    size_t _block_allocated = 0;
};
static std::map<BlockData*, BlockHeader*> _block_map;

void BlockManager::add_block(BlockData* data, BlockHeader* header){
    _block_map.insert(std::make_pair(data, header));
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

}; // namespace sql;