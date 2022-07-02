#include <memory/utility.h>

namespace sqrl {

extern "C" {
Block *_block_head = nullptr;
Block *_block_top = nullptr;
};
static size_t _memory_size = 0;

BlockAgent::BlockAgent(Block *block) : _block(block) {}
BlockAgent::BlockAgent(BlockAgent &&other) {
  _block = other._block;
  other._block = nullptr;
}
BlockAgent::BlockAgent(const BlockAgent &other) { _block = other._block; }
BlockAgent BlockAgent::operator=(BlockAgent &&other) {
  _block = other._block;
  other._block = nullptr;
}
bool BlockAgent::operator==(const BlockAgent &other) const {
  return _block == other._block;
}
size_t BlockAgent::size() const { return size_get(_block); }
void BlockAgent::set_size(size_t value) const {
  return size_set(_block, value);
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
  _memory_size += agent.size();
  agent.set_used(true);
}

void BlockManager::free_block(BlockData *data) {
  BlockHeader *header = (BlockHeader *)get_header(data);
  if (used((Block *)header)) {
    _memory_size -= size_get((Block *)header);
  }
  used_clear((Block *)header);
}

BlockAgent BlockManager::get_new_block(size_t size) {
  auto block = request_from_os(size);
  return BlockAgent(block);
}

void BlockManager::split_block(BlockAgent agent, size_t size) {
  split(agent._block, size);
  // edit memory size
}

BlockAgent BlockManager::get_head() { return BlockAgent(_block_head); }

BlockAgent BlockManager::get_top() { return BlockAgent(_block_top); }

void BlockManager::clear() {
  for (auto itr = _block_head; itr != _block_top; itr=itr->next) {
    used_clear(itr);
  }
  _memory_size = 0;
}

bool BlockManager::safe_check() {
  auto itr = _block_head;
  while (itr != nullptr) {
    if (itr == itr->next) {
      return false;
    }
    itr = itr->next;
  }
  return true;
}

size_t BlockViewer::memory_size() { return _memory_size; }

size_t BlockViewer::block_number() {
  size_t count = 0;
  for (auto itr = _block_head; itr != _block_top; itr=itr->next) {
    count++;
  }
  return count;
}
}; // namespace sqrl