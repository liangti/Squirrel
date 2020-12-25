#include <gtest/gtest.h>
#include <metaprogramming/types.h>

using namespace sql;

TEST(test_tuple, construct_empty_tuple){
    bool t1 = same_t<int, int>::value;
    bool t2 = same_t<int, char>::value;
    bool t3 = same_t<int, int*>::value;
    bool t4 = same_t<int, int&>::value;
    EXPECT_TRUE(t1);
    EXPECT_FALSE(t2);
    EXPECT_FALSE(t3);
    EXPECT_FALSE(t4);
}