#include <string/string.h>

namespace sql{

void _strcpy(char* char1, const char* char2){
    while(char2 != nullptr && *char2 != '\0'){
        
        *char1 = *char2;
        char1++;
        char2++;
    }
}

unsigned int _strlen(const char* input){
    unsigned int count = 0;
    while(*input != '\0'){
        count++;
        input++;
    }
    return count;
}

const char *String::c_str(){
    return s_data;
}

String::String(const char* str){
    if(str == nullptr){
        s_data = allocate(1);
        *s_data = '\0';
    }
    else {
        s_data = allocate(_strlen(str) + 1);
        _strcpy(s_data, str);
    }
}

String::~String(){
    deallocate(s_data);
}

String::String(const String& other){
    s_data = allocate(_strlen(other.s_data) + 1);
    _strcpy(s_data, other.s_data);
}

String &String::operator=(const String& other){
    if(this == &other){
        return *this;
    }
    if(s_data != nullptr){
        delete[] s_data;
    }
    s_data = allocate(_strlen(other.s_data) + 1);
    _strcpy(s_data, other.s_data);
    return *this;
}

String::String(String&& other){
    s_data = other.s_data;
    other.s_data = nullptr;
}

String &String::operator=(String&& other){
    if(this == &other){
        return *this;
    }
    if(s_data != nullptr){
        delete[] s_data;
    }
    s_data = other.s_data;
    other.s_data = nullptr;
    return *this;
}

String String::operator+(const String& other){
    char *temp = allocate(_strlen(s_data) + _strlen(other.s_data) + 1);
    char *ptr = s_data;
    char *ptr_temp = temp;
    while(*ptr != '\0'){
        *ptr_temp = *ptr;
        ptr_temp++;
        ptr++;
    }
    ptr = other.s_data;
    while(*ptr != '\0'){
        *ptr_temp = *ptr;
        ptr_temp++;
        ptr++;
    }
    return String(temp);

}

char* String::allocate(size_t size){
    return allocator.allocate(size);
}

void String::deallocate(char* data){
    if(data == nullptr){
        return;
    }
    allocator.deallocate(data);
}

};