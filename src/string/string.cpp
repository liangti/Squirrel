#include <string/string.h>

namespace sql {

static const int _strcmp_max = 100; // strcmp max length

void _strcpy(char *char1, const char *char2) {
  while (char2 != nullptr && *char2 != '\0') {

    *char1 = *char2;
    char1++;
    char2++;
  }
}

unsigned int _strlen(const char *input) {
  unsigned int count = 0;
  while (*input != '\0') {
    count++;
    input++;
  }
  return count;
}

bool _strcmp(const char *c1, const char *c2) {
  return _strncmp(c1, c2, _strcmp_max);
}

bool _strncmp(const char *c1, const char *c2, size_t len) {
  auto ptr1 = c1;
  auto ptr2 = c2;
  while (true) {
    if (len == 0) {
      break;
    }
    if (*ptr1 == '\0' || *ptr2 == '\0') {
      if (*ptr1 != '\0' || *ptr2 != '\0') {
        return false;
      }
      return true;
    }
    if (*ptr1 != *ptr2) {
      return false;
    }
    len--;
    ptr1++;
    ptr2++;
  }
  return true;
}

const char *String::c_str() { return s_data; }

size_t String::size() { return s_len; }

String::String(const char *str) {
  if (str == nullptr) {
    s_data = allocate(1);
    s_len = 0;
    *s_data = '\0';
  } else {
    s_len = _strlen(str);
    s_data = allocate(s_len + 1);
    _strcpy(s_data, str);
  }
}

String::~String() { deallocate(s_data); }

String::String(const String &other) {
  s_data = allocate(_strlen(other.s_data) + 1);
  _strcpy(s_data, other.s_data);
  s_len = other.s_len;
}

String &String::operator=(const String &other) {
  if (this == &other) {
    return *this;
  }
  if (s_data != nullptr) {
    delete[] s_data;
  }
  s_data = allocate(_strlen(other.s_data) + 1);
  _strcpy(s_data, other.s_data);
  s_len = other.s_len;
  return *this;
}

String::String(String &&other) {
  s_data = other.s_data;
  s_len = other.s_len;
  other.s_data = nullptr;
  other.s_len = 0;
}

String &String::operator=(String &&other) {
  if (this == &other) {
    return *this;
  }
  if (s_data != nullptr) {
    delete[] s_data;
  }
  s_data = other.s_data;
  s_len = other.s_len;
  other.s_data = nullptr;
  other.s_len = 0;
  return *this;
}

String String::operator+(const String &other) {
  char *temp = allocate(_strlen(s_data) + _strlen(other.s_data) + 1);
  char *ptr = s_data;
  char *ptr_temp = temp;
  while (*ptr != '\0') {
    *ptr_temp = *ptr;
    ptr_temp++;
    ptr++;
  }
  ptr = other.s_data;
  while (*ptr != '\0') {
    *ptr_temp = *ptr;
    ptr_temp++;
    ptr++;
  }
  return String(temp);
}

bool String::operator==(const String &other) {
  if (s_len != other.s_len) {
    return false;
  }
  return _strncmp(s_data, other.s_data, s_len);
}

char *String::allocate(size_t size) { return allocator.allocate(size); }

void String::deallocate(char *data) {
  if (data == nullptr) {
    return;
  }
  // deallocate only 1 block
  allocator.deallocate(data, 1);
}

}; // namespace sql