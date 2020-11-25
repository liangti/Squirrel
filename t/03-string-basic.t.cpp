#include<gtest/gtest.h>
#include <string/string.h>

using namespace sql;

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