#include<gtest/gtest.h>
#include <string/string.h>

using namespace sql;

TEST(test_string_utils, strcpy){
    char *c = new char[3];
    _strcpy(c, "abc");
    ASSERT_STREQ(c, "abc");
    delete[] c;
}
TEST(test_string_utils, strlen){
    char *c = new char[3];
    _strcpy(c, "ab");
    ASSERT_EQ(_strlen(c), 2);
    delete[] c;
}

TEST(test_string_basic, simple_constructor){
    String s("abc");
    ASSERT_STREQ(s.c_str(), "abc");
}

TEST(test_string_basic, copy_constructor){
    String s("abc");
    String ss(s);
    ASSERT_STREQ(ss.c_str(), "abc");
}

TEST(test_string_basic, move_constructor){
    String s("abc");
    String ss(std::move(s));
    ASSERT_STREQ(ss.c_str(), "abc");
    EXPECT_TRUE(s.c_str() == nullptr);
}

TEST(test_string_basic, copy_assignment){
    String s("abc");
    String ss = s;
    ASSERT_STREQ(ss.c_str(), "abc");
}

TEST(test_string_basic, move_assignment){
    String s("abc");
    String ss = std::move(s);
    ASSERT_STREQ(ss.c_str(), "abc");
    EXPECT_TRUE(s.c_str() == nullptr);
}

TEST(test_string_basic, copy_self_assignment){
    String s("abc");
    s = s;
    ASSERT_STREQ(s.c_str(), "abc");
}

TEST(test_string_basic, move_self_assignment){
    String s("abc");
    s = std::move(s);
    ASSERT_STREQ(s.c_str(), "abc");
}