#ifndef INCLUDED_SMART_POINTERS_H
#define INCLUDED_SMART_POINTERS_H

namespace sql{

template<typename T>
class shared_ptr{
private:
    int* count;
    T* ptr;
public:
    shared_ptr(T* _ptr){
        ptr = _ptr;
        count = new int(1);
    }

    shared_ptr(const shared_ptr<T> &other){
        count = other.count;
        ptr = other.ptr;
        (*count)++;
    }

    ~shared_ptr(){
        if((*count) == 1){
            delete count;
            delete ptr;
        }
    }
    T* operator->(){
        return ptr;
    }
    shared_ptr<T>& operator=(const shared_ptr<T>& rhs){
        count = rhs.count;
        ptr = rhs.ptr;
        (*count)++;
    }

    T* operator*(){
        return ptr;
    }
};

}; // namespace sql

#endif