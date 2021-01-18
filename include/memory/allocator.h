#ifndef INCLUDED_MEMORY_ALLOCATOR_H
#define INCLUDED_MEMORY_ALLOCATOR_H
#include <memory/allocator_utils.h>

namespace sql{

class AllocatorImpl;

class AllocatorBase{
private:
    AllocatorImpl* impl;
public:
    AllocatorBase();
    internal::word_t* allocate(size_t);
    void deallocate(internal::word_t*);
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
    virtual void deallocate(T* t){
        base.deallocate((internal::word_t*)t);
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