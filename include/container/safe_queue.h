#ifndef INCLUDED_SAFE_QUEUE_H
#define INCLUDED_SAFE_QUEUE_H

#include <atomic>
#include <container/queue.h>
#include <mutex>
#include <utility>

#define MAX_QUEUE_NUM 50

namespace sqrl {

template <typename T> class SafeQueue : Queue<T> {
public:
  SafeQueue(size_t queue_size = MAX_QUEUE_NUM) { _init_space(queue_size); }

  ~SafeQueue() {
    std::unique_lock<std::mutex> guard(lock);
    _deallocate(_root);
  }

  inline bool empty() { return _count == 0; }
  inline bool full() { return _count == _capacity; }
  void push(const T &t) {
    std::unique_lock<std::mutex> guard(lock);
    size_t temp_tail = _tail;
    T *current = _root + _tail;
    *current = t;
    _count++;
    _tail = (temp_tail + 1) % _capacity;
  }
  void pop() {
    std::unique_lock<std::mutex> guard(lock);
    size_t temp_head = _head;
    _count--;
    _head = (temp_head + 1) % _capacity;
  }
  T &front() const {
    T *front = _root + _head;
    return *front;
  }
  template <typename... Args> void emplace(Args &&...args) {
    std::unique_lock<std::mutex> guard(lock);
    size_t temp_tail = _tail;
    T *current = new (_root + _tail) T(std::forward<Args>(args)...);
    _tail = (temp_tail + 1) % _capacity;
  }
  size_t size() {
    if (empty()) {
      return 0;
    }
    if (full()) {
      return _capacity;
    }
    return _tail < _head ? (_tail + _capacity - _head) : (_tail - _head);
  }

private:
  T *_root;
  std::atomic<size_t> _head;
  std::atomic<size_t> _tail;
  std::atomic<size_t> _count;
  std::atomic<size_t> _capacity;
  std::mutex lock;
  sqrl::Allocator<T> _allocator;

  void _init_space(size_t capacity) {
    _capacity = capacity;
    _root = _allocate(capacity);
    _head = 0;
    _tail = 0;
    _count = 0;
  }

  // TODO: does it work fine with objects with virtual?
  T *_allocate(size_t capacity) {
    return _allocator.allocate(capacity + BUFFER);
  }

  void _deallocate(T *begin) { _allocator.deallocate(begin, 1); }
};

/*
Still in experiment

This Queue uses CAS to implement spinlock to achive threadsafe

For adding new element it needs to compete the follwing items in given order:
- compete for size(_count++)
- compete for write address(_tail++)
- compete for write access(state=free)

For poping element it also needs to compete the following items in given order:
- compete for size(_count--)
- compete for pop address(_head++)
- compete for write access(state=ready)
*/
template <typename T> class BlockFreeQueue {
public:
  BlockFreeQueue(size_t queue_size = MAX_QUEUE_NUM) { _init_space(queue_size); }
  ~BlockFreeQueue() {
    _deallocate();
    _deallocate();
  }
  inline bool empty() { return _count == 0; }
  inline bool full() { return _count == _capacity; }

  size_t wait_for_push() {
    size_t temp_count;
    size_t new_count;
    size_t temp_tail;
    size_t new_tail;
    // compete for size
    do {
      temp_count = _count;
      new_count = temp_count + 1;
    } while (new_count <= _capacity &&
             !_count.compare_exchange_strong(temp_count, new_count));
    // compete for a location
    do {
      temp_tail = _tail;
      new_tail = (temp_tail + 1) % _capacity;
    } while (!_tail.compare_exchange_strong(temp_tail, new_tail));
    // compete for a location to be ready to write
    State *current_state = _state_root + temp_tail;
    state free_state = free;
    while (!(*current_state).compare_exchange_strong(free_state, working)) {
      free_state = free;
    }
    return temp_tail;
  }

  void push(const T &t) {
    size_t temp_tail = wait_for_push();
    T *current_data = _data_root + temp_tail;
    *current_data = t;
    State *current_state = _state_root + temp_tail;
    *current_state = ready;
  }

  template <typename... Args> void emplace(Args &&...args) {
    size_t temp_tail = wait_for_push();
    T *current = new (_data_root + temp_tail) T(std::forward<Args>(args)...);
    State *current_state = _state_root + temp_tail;
    *current_state = ready;
  }

  void pop() {
    size_t temp_count;
    size_t new_count;
    size_t temp_head;
    size_t new_head;
    do {
      temp_count = _count;
      new_count = temp_count - 1;
    } while (new_count >= 0 &&
             !_count.compare_exchange_strong(temp_count, new_count));
    do {
      temp_head = _head;
      new_head = (temp_head + 1) % _capacity;
    } while (!_head.compare_exchange_strong(temp_head, new_head));
    State *current_state = _state_root + temp_head;
    state ready_state = ready;
    // wait for the address to ready
    // maybe last push()/emplace() haven't finished
    while (!(*current_state).compare_exchange_strong(ready_state, free)) {
      ready_state = ready;
    }
  }

  size_t size() { return _count; }

  // TODO size, empty, full
private:
  enum state { free, working, ready };
  typedef std::atomic<state> State;
  std::atomic<size_t> _head;
  std::atomic<size_t> _tail;
  std::atomic<size_t> _count;
  std::atomic<size_t> _capacity;
  T *_data_root;
  State *_state_root;
  sqrl::Allocator<T> _allocator_t;
  sqrl::Allocator<State> _allocator_s;

  void _init_space(size_t capacity) {
    _capacity = capacity;
    _data_root = _allocator_t.allocate(capacity + BUFFER);
    _state_root = _allocator_s.allocate(capacity + BUFFER);
    _head = 0;
    _tail = 0;
    _count = 0;
    // mark all blocks state to be free at very beginning
    for (size_t i = 0; i < capacity; i++) {
      State *temp = _state_root + i;
      *temp = free;
    }
  }

  void _deallocate() {
    _allocator_t.deallocate(_data_root, 1);
    _allocator_s.deallocate(_state_root, 1);
  }
};

} // namespace sqrl
#endif