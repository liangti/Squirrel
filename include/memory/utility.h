#ifndef INCLUDED_UTILITY_H
#define INCLUDED_UTILITY_H

#include <memory/block.h>
#include <map>

namespace sql{

using BlockData = word_t;


class BlockManager{
public:
    BlockManager() = default;
    ~BlockManager() = default;
    void add_block(BlockData*, BlockHeader*);
    bool valid_block(BlockData*);
    void free_block(BlockData*);
    BlockHeader* get_block_header(BlockData*);
    Block* get_head();
    Block* get_top();
    void clear();
};

/*
Block Viewer
Different from Allocator, Viewer does not participate in Block
manipulation. It only view the memory block status
*/
class BlockViewer{
public:
    BlockViewer() = default;
    size_t num();
    size_t size();
};

}; // namespace sql

#endif