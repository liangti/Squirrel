#ifndef INCLUDED_STRING_H
#define INCLUDED_STRING_H

#include <memory/allocator.h>
namespace sql{

void _strcpy(char*, const char*);
unsigned int _strlen(const char*);
bool _strcmp(const char*, const char*);
bool _strncmp(const char*, const char*, size_t);

class String{
public:
    String(const char*);
    ~String();
    String(const String&);
    String(String&&);
    String& operator=(const String&);
    String& operator=(String&&);
    String operator+(const String&);
    bool operator==(const String&);
    const char* c_str();
    size_t size();
private:
    char *s_data;
    size_t s_len;
    sql::Allocator<char> allocator;
    char* allocate(size_t);
    void deallocate(char*);
};

}; // namespace sql

#endif