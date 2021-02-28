#ifndef INCLUDED_UTILITY_H
#define INCLUDED_UTILITY_H

#include <memory/block.h>
#include <map>

namespace sql{

using BlockData = word_t;


class BlockAgent{
friend class BlockManager;
private:
    Block* _block = nullptr;
public:
    BlockAgent() = default;
    BlockAgent(Block* block);
    BlockAgent(BlockAgent&& other);
    BlockAgent(const BlockAgent& other);
    BlockAgent operator=(BlockAgent&& other);
    bool operator==(const BlockAgent& other);
    size_t size();
    void set_size(size_t value);
    bool is_used();
    void set_used(bool value);
    BlockData* get_data();
    bool null();
    void next();
};

class BlockManager{
public:
    BlockManager() = default;
    ~BlockManager() = default;
    void add_block(BlockAgent, bool);
    void free_block(BlockData*);
    void split_block(BlockAgent, size_t);
    BlockAgent get_new_block(size_t);
    BlockAgent get_head();
    BlockAgent get_top();
    void clear();
    bool safe_check();
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