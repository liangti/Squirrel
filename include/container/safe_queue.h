#ifndef INCLUDED_SAFE_QUEUE_H
#define INCLUDED_SAFE_QUEUE_H

#include <utility>
#include <mutex>
#include <atomic>
#include <container/queue.h>

#define MAX_QUEUE_NUM 50

namespace sql{

template<typename T>
class SafeQueue: Queue<T>{
public:
    SafeQueue(size_t queue_size=MAX_QUEUE_NUM){
        _init_space(queue_size);
    }
    bool empty(){
        return _count == 0;
    }
    bool full(){
        return _count == _capacity;
    }
    void push(const T& t){
        std::unique_lock<std::mutex> guard(lock);
        size_t temp_tail = _tail;
        T* current = _root + _tail;
        *current = t;
        _count++;
        _tail = (temp_tail + 1) % _capacity;
    }
    void pop(){
        std::unique_lock<std::mutex> guard(lock);
        size_t temp_head = _head;
        _count--;
        _head = (temp_head + 1) % _capacity;
    }
    T& front() const{
        T* front = _root + _head;
        return *front;
    }
    template<typename...Args>
    void emplace(Args&&... args){
        std::unique_lock<std::mutex> guard(lock);
        size_t temp_tail = _tail;
        T* current = new(_root + _tail) T(std::forward<Args>(args)...);
        _tail = (temp_tail + 1) % _capacity;
    }
    size_t size(){
        if(empty()){
            return 0;
        }
        if(full()){
            return _capacity;
        }
        return _tail < _head ? (_tail + _capacity - _head) : (_tail - _head);
    }
private:
    T* _root;
    std::atomic<size_t> _head;
    std::atomic<size_t> _tail;
    std::atomic<size_t> _count;
    std::atomic<size_t> _capacity;
    std::mutex lock;

    void _init_space(size_t capacity){
        _capacity = capacity;
        _root = _allocate(capacity);
        _head = 0;
        _tail = 0;
        _count = 0;
    }

    // TODO: does it work fine with objects with virtual?
    T* _allocate(size_t capacity){
        return reinterpret_cast<T*>(operator new[](capacity * sizeof(T) + BUFFER));
    }

};


} // namespace sql
#endif