#include <gtest/gtest.h>

#define SQL_CHECK_ATTRIBUTE x
#include <metaprogramming/attribute.h>

class A{
public:
    int x;
};

class B{};

TEST(test_has_attribute, has_attrbute){
    EXPECT_TRUE(sql::checkattr<A>::value); // has member x
}

TEST(test_has_attribute, has_no_attribute){
    EXPECT_FALSE(sql::checkattr<B>::value); // doesn't have member x
}