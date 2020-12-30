#ifndef INCLUDED_VECTOR_H
#define INCLUDED_VECTOR_H

#include <initializer_list>

namespace sql{

template<typename T>
class Vector{
public:
    // default constructor
    Vector(){
        _init_space();
    }

    // initializer list
    Vector(std::initializer_list<T> l){
        _init_space();
        for(auto itr = l.begin(); itr != l.end(); itr++){
            push_back(*itr);
        }
    }

    // shallow copy
    Vector(const Vector<T>& v){
        _init_space();
        for(auto itr = v.cbegin(); itr != v.cend(); itr++){
            push_back(*itr);
        }
    }

    // construct by iterator
    Vector(const T* vbegin, const T* vend){
        _init_space();
        if(vend <= vbegin){
            return;
        }
        for(auto itr = vbegin; itr != vend; itr++){
            push_back(*itr);
        }
    }

    inline T* begin() const{
        return _begin;
    }

    inline T* end() const{
        return _begin + _top;
    }

    inline const T* cbegin() const{
        return _begin;
    }

    inline const T* cend() const{
        return _begin + _top;
    }

    bool empty(){
        return _top == 0;
    }

    void push_back(const T& t){
        _check_size(_top + 1);
        T* current = _begin + _top;
        *current = t;
        _top++;
    }

    void pop_back(){
        if(empty()){
            return;
        }
        _top--;
    }

    template<typename... Args>
    void emplace_back(Args&&... args){
        _check_size(_top + 1);
        // placement new is to create an object in existing address space
        T* current = new(_begin + _top) T(std::forward<Args>(args)...);
        _top++;
    }

    inline size_t size(){
        return _top;
    }

    inline T& operator[](size_t idx){
        return *(_begin + idx);
    }

    void resize(size_t new_capacity){
        _capacity = new_capacity;
        T* new_begin = _allocate(new_capacity);

        // copy old elements
        for(size_t i = 0; i < _top; i++){
            T* current = new_begin + i;
            T* previous = _begin + i;
            *current = *previous;
        }
        // delete old stuffs
        delete[] _begin;
        _begin = new_begin;
    }


protected:
    size_t _capacity;
    size_t _top;
    T* _begin;
    T* _end;
    void _init_space(size_t capacity=50){
        _capacity = capacity;
        _top = 0;
        _begin = _allocate(capacity);
        _end = _begin + _capacity;
    }

    // TODO: does it work fine with objects with virtual?
    T* _allocate(size_t capacity){
        return reinterpret_cast<T*>(operator new[](capacity * sizeof(T)));
    }
    
    void _check_size(size_t required){
        if(required > _capacity){
            resize(_capacity * 2);
        }
    }
};

}; // using namespace sql

#endif