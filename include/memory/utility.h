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
    void add_block(BlockData*, BlockHeader*, bool);
    void free_block(BlockData*);
    void split_block(Block*, size_t);
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
    size_t size();
};

}; // namespace sql

#endif