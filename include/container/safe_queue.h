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
    inline bool empty(){
        return _count == 0;
    }
    inline bool full(){
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


/*
Still in experiment
*/
template<typename T>
class BlockFreeQueue{
public:
    BlockFreeQueue(size_t queue_size=MAX_QUEUE_NUM){
        _init_space(queue_size);
    }
    inline bool empty(){
        return _count == 0;
    }
    inline bool full(){
        return _count == _capacity;
    }

    size_t wait_for_push(){
        size_t temp_tail;
        size_t new_tail;
        // compete for a location
        do{
            temp_tail = _tail;
            new_tail = (temp_tail + 1) % _capacity;
        } while(!_tail.compare_exchange_strong(temp_tail, new_tail));
        _count++; 
        // compete for a location to be ready to write
        State* current_state = _state_root + temp_tail;
        state free_state = free;
        while(!(*current_state).compare_exchange_strong(free_state, working)){
            free_state = free;
        }
        return temp_tail;
    }

    void push(const T& t){
        size_t temp_tail = wait_for_push();
        T* current_data = _data_root + temp_tail;
        *current_data = t;
        State* current_state = _state_root + temp_tail;
        *current_state = ready;
    }

    template<typename...Args>
    void emplace(Args&&... args){
        size_t temp_tail = wait_for_push();
        T* current = new(_data_root + temp_tail) T(std::forward<Args>(args)...);
        State* current_state = _state_root + temp_tail;
        *current_state = ready;
    }

    void pop(){
        size_t temp_head;
        size_t new_head;
        State* current_state = _state_root + temp_head;
        state ready_state = ready;
        // wait for the address to ready
        // maybe last push()/emplace() haven't finished
        while(!(*current_state).compare_exchange_strong(ready_state, free)){
            ready_state = ready;
        }
        do{
            temp_head = _head;
            new_head = _head = (temp_head + 1) % _capacity;
        } while(!_head.compare_exchange_strong(temp_head, new_head));
        _count--;
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

    // TODO size, empty, full
private:
    enum state{free, working, ready};
    typedef std::atomic<state> State;
    std::atomic<size_t> _head;
    std::atomic<size_t> _tail;
    std::atomic<size_t> _count;
    std::atomic<size_t> _capacity;
    T* _data_root;
    State* _state_root;

    void _init_space(size_t capacity){
        _capacity = capacity;
        _data_root = _allocate<T>(sizeof(T), capacity);
        _state_root = _allocate<State>(sizeof(State), capacity);
        _head = 0;
        _tail = 0;
        _count = 0;
        // mark all blocks state to be free at very beginning
        for(size_t i = 0; i < capacity; i++){
            State* temp = _state_root + i;
            *temp = free;
        }
    }

    // TODO: does it work fine with objects with virtual?
    template<typename U>
    U* _allocate(size_t unit_size, size_t capacity){
        return reinterpret_cast<U*>(operator new[](capacity * unit_size + BUFFER));
    }
};

} // namespace sql
#endif