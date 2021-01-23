#ifndef INCLUDED_MEMORY_ALLOCATOR_H
#define INCLUDED_MEMORY_ALLOCATOR_H
#include <memory/block.h>

namespace sql{

class AllocatorImpl;

class AllocatorBase{
private:
    AllocatorImpl* impl;
public:
    AllocatorBase();
    word_t* allocate(size_t);
    void deallocate(word_t*);
    void clear();
    size_t allocated_size();
};

// allocator interface
template<typename T>
class Allocator{
private:
    int x;
public:
    AllocatorBase base;
    virtual T* allocate(size_t size){
        return (T*)base.allocate(size * sizeof(T));
    }
    virtual void deallocate(T* t, size_t size){
        size_t ptr = 0;
        // call destructor first;
        while(ptr != size){
            (t + ptr)->~T();
            ptr++;
        }
        base.deallocate((word_t*)t);
    }
    virtual void clear(){
        base.clear();
    }
    virtual ~Allocator(){
        clear();
    }
    size_t allocated_size(){
        return base.allocated_size();
    }
};

}; // namespace sql

#endif