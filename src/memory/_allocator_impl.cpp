#include <memory/_allocator_impl.h>

namespace sqrl {

extern "C" {
Block *_block_head = nullptr;
Block *_block_top = nullptr;
};
static size_t _memory_used_size = 0;
static size_t _memory_requested_size = 0;

BlockAgent::BlockAgent(Block *block) : _block(block) {}
BlockAgent::BlockAgent(BlockAgent &&other) {
  _block = other._block;
  other._block = nullptr;
}
BlockAgent::BlockAgent(const BlockAgent &other) { _block = other._block; }

BlockAgent BlockAgent::operator=(BlockAgent &&other) {
  _block = other._block;
  other._block = nullptr;
  return *this;
}

bool BlockAgent::operator==(const BlockAgent &other) const {
  return _block == other._block;
}
size_t BlockAgent::size() const { return get_size(_block); }
void BlockAgent::set_size(size_t value) const {
  // set_size from block.c
  return ::set_size(_block, value);
}
bool BlockAgent::is_used() const { return used(_block); }
void BlockAgent::set_used(bool value) {
  if (value) {
    used_set(_block);
  } else {
    used_clear(_block);
  }
}
BlockData *BlockAgent::get_data() { return _block->data; }
bool BlockAgent::null() const { return _block == nullptr; }
void BlockAgent::next() { _block = _block->next; }

// mark block used and add to chain if the block is not there yet.
void BlockManager::add_block(BlockAgent agent, bool is_new) {
  auto block = agent._block;
  // for small block add to linkedlist for reuse
  // for large block don't, see once release it will be returned back to os
  if (agent._block->_size < SQRL_ALLOCATOR_MMAP_THRESHOLD) {
    if (is_new) {
      block->next = nullptr;
      if (_block_head == nullptr) {
        _block_head = block;
      }
      // Chain the blocks
      if (_block_top != nullptr) {
        _block_top->next = block;
      }
      _block_top = block;
    }
  }
  _memory_used_size += agent.size();
  agent.set_used(true);
}

void BlockManager::free_data(BlockData *data) {
  BlockHeader *header = (BlockHeader *)get_header(data);
  if (header->_size < SQRL_ALLOCATOR_MMAP_THRESHOLD) {
    if (used((Block *)header)) {
      _memory_used_size -= get_size((Block *)header);
    }
    release_block(get_block(data));
  } else {
    _memory_used_size -= get_size((Block *)header);
    _memory_requested_size -= get_size((Block *)header);
    release_large_block_to_os(get_block(data));
  }
}

BlockAgent BlockManager::get_new_block(size_t size) {
  Block *block;
  if (size < SQRL_ALLOCATOR_MMAP_THRESHOLD) {
    block = request_block_from_os(size);
  } else {
    block = request_large_block_from_os(size);
  }
  _memory_requested_size += alloc_size(size);
  return BlockAgent(block);
}

void BlockManager::split_block(BlockAgent agent, size_t size) {
  split(agent._block, size);
  // edit memory size
}

BlockAgent BlockViewer::get_head() { return BlockAgent(_block_head); }

BlockAgent BlockViewer::get_top() { return BlockAgent(_block_top); }

void BlockManager::clear() {
  for (auto itr = _block_head; itr != _block_top; itr = itr->next) {
    used_clear(itr);
  }
  _memory_used_size = 0;
}

bool BlockViewer::safe_check() {
  auto itr = _block_head;
  while (itr != nullptr) {
    if (itr == itr->next) {
      return false;
    }
    itr = itr->next;
  }
  return true;
}

size_t BlockViewer::memory_size() { return _memory_used_size; }

size_t BlockViewer::memory_used_size() { return _memory_used_size; }

size_t BlockViewer::memory_requested_size() { return _memory_requested_size; }

size_t BlockViewer::block_number() {
  size_t count = 0;
  for (auto itr = _block_head; itr != _block_top; itr = itr->next) {
    count++;
  }
  return count;
}

// AllocatorImpl

BlockAgent AllocatorImpl::find_free_block(size_t request_size) {
  auto block = viewer.get_head();
  auto size = alloc_size(request_size);

  while (!block.null()) {
    if (block.is_used() || block.size() < size || block.size() >= size * 2) {
      block.next();
      continue;
    }
    // TODO split the large block
    // maybe in Reclaimer
    return block;
  }
  return BlockAgent();
}

word_t *AllocatorImpl::allocate(size_t size) {
  bool is_new = false;
  BlockAgent block = find_free_block(size);
  if (block.null()) {
    block = manager.get_new_block(size);
    is_new = true;
  }
  manager.add_block(block, is_new);
  return block.get_data();
}

void AllocatorImpl::deallocate(word_t *data) { manager.free_data(data); }

}; // namespace sqrl