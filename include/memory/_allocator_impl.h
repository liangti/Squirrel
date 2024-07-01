#ifndef INCLUDED_ALLOCATOR_IMPL_H
#define INCLUDED_ALLOCATOR_IMPL_H

/*
Internal implementation of allocator
Not suppose to use directlly
*/

#include <map>
#include <memory/block.h>

// From M_MMAP_THRESHOLD from glibc
#define SQRL_ALLOCATOR_MMAP_THRESHOLD 131072

namespace sqrl {

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
  // free a block from given data
  void free_data(BlockData *);
  void split_block(BlockAgent, size_t);
  BlockAgent get_new_block(size_t);
  void clear();
};

/*
Block Viewer
Different from Allocator, Viewer does not participate in Block
manipulation. It only view the memory block status
*/
class BlockViewer {
public:
  BlockViewer() = default;
  // for allocator users that don't care much implementation details
  size_t memory_size();
  // used != requested, only when small block requested from os but not used
  size_t memory_used_size();
  size_t memory_requested_size();

  size_t block_number();
  BlockAgent get_head();
  BlockAgent get_top();
  bool safe_check();
};

class AllocatorImpl {
private:
  BlockManager manager;
  BlockViewer viewer;
  BlockAgent find_free_block(size_t requested_size);

public:
  AllocatorImpl() = default;
  word_t *allocate(size_t);
  void deallocate(word_t *data);
  ~AllocatorImpl() = default;
};

}; // namespace sqrl

#endif