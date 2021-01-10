#ifndef INCLUDED_SMART_POINTERS_H
#define INCLUDED_SMART_POINTERS_H

namespace sql{

class _allocator_base{
public:
    template<typename T>
    static void clean(T* data){
        delete data;
    }
};

// should not be used externally

class _counter_allocator: _allocator_base{
private:
    static size_t _free_size;
public:
    template<typename T>
    static void clean(T* data){
        _allocator_base::clean(data);
        _free_size += sizeof(T);
    }
    static size_t get_free_size(){
        return _free_size;
    }
    static void reset_free_size(){
        _free_size = 0;
    }
};

// smart pointers expose to users

template<typename T, class LocalAllocator=_allocator_base>
class shared_ptr{
private:
    int* count;
    T* ptr;
public:
    shared_ptr(T* _ptr)
    : ptr(_ptr), count(new int(1)){
    }

    template<typename Allocator>
    shared_ptr(shared_ptr<T, Allocator> &other){
        count = other.count;
        ptr = other.ptr;
        (*count)++;
    }

    ~shared_ptr(){
        if((*count) == 1){
            LocalAllocator::clean(count);
            LocalAllocator::clean(ptr);
        }
        else{
            (*count)--;
        }
    }
    T* operator->(){
        return ptr;
    }

    template<typename Allocator>
    shared_ptr<T, LocalAllocator>& operator=(shared_ptr<T, Allocator>& rhs){
        count = rhs.count;
        ptr = rhs.ptr;
        if(this != &rhs){
            (*count)++;
        }
        return *this;
    }

    T& operator*(){
        return *ptr;
    }

    void reset(){
        delete count;
        delete ptr;
    }

    int get_count(){
        return *count;
    }
};

}; // namespace sql

#endif