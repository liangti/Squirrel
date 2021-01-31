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
    BlockHeader* get_block_header(BlockData*);
};

}; // namespace sql

#endif