#ifndef INCLUDED_STRING_H
#define INCLUDED_STRING_H

namespace sql{

class String{
public:
    String(const char*);
    ~String();
    String(const String&);
    String(String&&);
    String& operator=(const String&);
    String& operator=(String&&);
    char* c_str();
private:
    char *s_data;
};

}; // namespace sql

#endif