#ifndef INCLUDED_UTILITY_H
#define INCLUDED_UTILITY_H

#include <map>
#include <memory/block.h>

namespace sql {

using BlockData = word_t;

class BlockAgent {
  friend class BlockManager;

private:
  Block *_block = nullptr;

public:
  BlockAgent() = default;
  BlockAgent(Block *block);
  BlockAgent(BlockAgent &&other);
  BlockAgent(const BlockAgent &other);
  BlockAgent operator=(BlockAgent &&other);
  bool operator==(const BlockAgent &other) const;
  size_t size() const;
  void set_size(size_t value) const;
  bool is_used() const;
  void set_used(bool value);
  BlockData *get_data();
  bool null() const;
  void next();
};

class BlockManager {
public:
  BlockManager() = default;
  ~BlockManager() = default;
  void add_block(BlockAgent, bool);
  void free_block(BlockData *);
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
class BlockViewer {
public:
  BlockViewer() = default;
  size_t size();
};

}; // namespace sql

#endif