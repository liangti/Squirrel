#include <gtest/gtest.h>

#define SQL_CHECK_ATTRIBUTE x
#include <metaprogramming/attribute.h>

class A{
public:
    int x;
};

TEST(test_has_attribute, has_attrbute){
    EXPECT_TRUE(sql::checkattr<A>::value);
}