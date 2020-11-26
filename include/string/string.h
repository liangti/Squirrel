#ifndef INCLUDED_STRING_H
#define INCLUDED_STRING_H

namespace sql{

void _strcpy(char*, const char*);
unsigned int _strlen(const char*);

class String{
public:
    String(const char*);
    ~String();
    String(const String&);
    String(String&&);
    String& operator=(const String&);
    String& operator=(String&&);
    String operator+(const String&);
    const char* c_str();
private:
    char *s_data;
};

}; // namespace sql

#endif