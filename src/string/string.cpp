#include <string/string.h>
#include <cstring>

namespace sql{

char *String::c_str(){
    return s_data;
}

String::String(const char* str){
    if(str == nullptr){
        s_data = new char[1];
        *s_data = '\0';
    }
    else {
        s_data = new char[strlen(str) + 1];
        strcpy(s_data, str);
    }
}

String::~String(){
    delete[] s_data;
}

String::String(const String& other){
    s_data = new char[strlen(other.s_data) + 1];
    strcpy(s_data, other.s_data);
}

String &String::operator=(const String& other){
    if(this == &other){
        return *this;
    }
    if(s_data != nullptr){
        delete[] s_data;
    }
    s_data = new char[strlen(other.s_data) + 1];
    strcpy(s_data, other.s_data);
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
}

};