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

// smart pointers expose to users

template<typename T, class LocalAllocator=_allocator_base>
class unique_ptr{
private:
    T* ptr;
public:
    unique_ptr(T* _ptr): ptr(_ptr){
    }

    template<typename Allocator>
    unique_ptr(unique_ptr<T, Allocator> &other){
        ptr = other.ptr;
        if(this != &other){
            other.ptr = nullptr;
        }
    }
    template<typename Allocator>
    unique_ptr<T, LocalAllocator>& operator=(unique_ptr<T, Allocator> &other){
        ptr = other.ptr;
        if(this != &other){
            other.ptr = nullptr;
        }
        return *this;
    }

    ~unique_ptr(){
        LocalAllocator::clean(ptr);
    }
    
    T* operator->(){
        return ptr;
    }
    
    T& operator*(){
        return *ptr;
    }

    bool is_null(){
        return ptr == nullptr;
    }
};

template<typename T>
class weak_ptr;

template<typename T, class LocalAllocator=_allocator_base>
class shared_ptr{
    friend weak_ptr<T>;
private:
    int* count;
    T* ptr;
public:
    shared_ptr(T* _ptr): ptr(_ptr), count(new int(1)){
    }

    template<typename Allocator>
    shared_ptr(shared_ptr<T, Allocator> &other){
        count = other.count;
        ptr = other.ptr;
        if(this != &other){
            (*count)++;
        }
    }

    ~shared_ptr(){
        if((*count) == 1){
            reset();
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
        LocalAllocator::clean(count);
        LocalAllocator::clean(ptr);
    }

    int get_count(){
        return *count;
    }
};

template<typename T>
class weak_ptr{
private:
    T* ptr;
    int* count;
public:
    weak_ptr(shared_ptr<T>& sp){
        ptr = sp.ptr;
        count = sp.count;
    }
    weak_ptr<T>& operator=(shared_ptr<T>& sp){
        ptr = sp.ptr;
        count = sp.count;
        return *this;
    }
    T& operator*(){
        return *ptr;
    }

    T* operator->(){
        return ptr;
    }

    int get_count(){
        return *count;
    }
};

}; // namespace sql

#endif