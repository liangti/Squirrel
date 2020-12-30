#ifndef INCLUDED_QUEUE_H
#define INCLUDED_QUEUE_H

#define MAX_QUEUE_NUM 50

#include <utility>

namespace sql{

template<typename T>
class Queue{
public:
    // constructor
    Queue(size_t queue_size=MAX_QUEUE_NUM){
        _init_space(queue_size);
    }

    inline bool empty(){
        return _count == 0;
    }

    inline bool full(){
        return _count == _capacity;
    }

    void push(const T& t){
        T* current = _root + _tail;
        *current = t;
        _push();
    }

    void pop(){
        _pop();
    }

    T& front() const{
        T* front = _root + _head;
        return *front;
    }

    template<typename... Args>
    void emplace(Args&&... args){
        T* current = new(_root + _tail) T(std::forward<Args>(args)...);
        _push();
    }

    inline size_t size(){
        return _count;
    }

    // do not allow resize

private:
    T* _root;
    size_t _head;
    size_t _tail;
    size_t _count;
    size_t _capacity;

    void _init_space(size_t capacity){
        _capacity = capacity;
        _root = _allocate(capacity);
        _head = 0;
        _tail = 0;
        _count = 0;
    }

    // TODO: does it work fine with objects with virtual?
    T* _allocate(size_t capacity){
        return reinterpret_cast<T*>(operator new[](capacity * sizeof(T)));
    }

    inline void _push(){
        _count++;
        _tail = (_tail + 1) % _capacity;
    }

    inline void _pop(){
        _count--;
        _head = (_head + 1) % _capacity;
    }

};

}; // namespace sql

#endif