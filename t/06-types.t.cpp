#include <gtest/gtest.h>
#include <metaprogramming/types.h>

using namespace sql;

TEST(test_types, check_type_equivalent){
    bool t1 = same_t<int, int>::value;
    bool t2 = same_t<int, double>::value;
    bool t3 = same_t<int, int*>::value;
    bool t4 = same_t<int, int&>::value;
    bool t5 = same_t<int, int&&>::value;
    bool t6 = same_t<int, const int>::value;
    bool t7 = same_t<int, volatile int>::value;
    EXPECT_TRUE(t1);
    EXPECT_FALSE(t2);
    EXPECT_FALSE(t3);
    EXPECT_FALSE(t4);
    EXPECT_FALSE(t5);
    EXPECT_FALSE(t6);
    EXPECT_FALSE(t7);
}

TEST(test_types, remove_const_and_volatile){
    bool t1 = same_t<int, sql::remove_cv<int>::type>::value;
    bool t2 = same_t<int, sql::remove_cv<const int>::type>::value;
    bool t3 = same_t<int, sql::remove_cv<volatile int>::type>::value;
    bool t4 = same_t<int, sql::remove_cv<const volatile int>::type>::value;
    EXPECT_TRUE(t1);
    EXPECT_TRUE(t2);
    EXPECT_TRUE(t3);
    EXPECT_TRUE(t4);
}

TEST(test_types, conditional_choose_types){
    using type1 = sql::conditional<true, int, double>::type;
    using type2 = sql::conditional<false, int, double>::type;
    bool t1 = same_t<type1, int>::value;
    bool t2 = same_t<type2, double>::value;
    EXPECT_TRUE(t1);
    EXPECT_TRUE(t2);
}

TEST(test_types, remove_reference){
    using type1 = sql::remove_reference<int>::type;
    using type2 = sql::remove_reference<int&>::type;
    using type3 = sql::remove_reference<int&&>::type;
    bool t1 = same_t<type1, int>::value;
    bool t2 = same_t<type2, int>::value;
    bool t3 = same_t<type3, int>::value;
    EXPECT_TRUE(t1);
    EXPECT_TRUE(t2);
    EXPECT_TRUE(t3);
}

TEST(test_types, remove_array){
    using type1 = sql::remove_array<int>::type;
    using type2 = sql::remove_array<int[]>::type;
    using type3 = sql::remove_array<int[3]>::type;
    bool t1 = same_t<type1, int>::value;
    bool t2 = same_t<type2, int>::value;
    bool t3 = same_t<type3, int>::value;
    EXPECT_TRUE(t1);
    EXPECT_TRUE(t2);
    EXPECT_TRUE(t3);
}

TEST(test_types, is_array){
    bool t1 = sql::is_array<int>::value;
    bool t2 = sql::is_array<int[]>::value;
    bool t3 = sql::is_array<int[3]>::value;
    EXPECT_FALSE(t1);
    EXPECT_TRUE(t2);
    EXPECT_TRUE(t3);
}

TEST(test_types, is_function){
    bool t1 = sql::is_function<int>::value;
    bool t2 = sql::is_function<int(int)>::value;
    EXPECT_FALSE(t1);
    EXPECT_TRUE(t2);
}

TEST(test_types, decay_types){
    using type1 = sql::decay<int&>::type;
    using type2 = sql::decay<int&&>::type;
    using type3 = sql::decay<const int>::type;
    using type4 = sql::decay<int[]>::type;
    using type5 = sql::decay<int(int)>::type;
    bool t1 = same_t<type1, int>::value;
    bool t2 = same_t<type2, int>::value;
    bool t3 = same_t<type3, int>::value;
    bool t4 = same_t<type4, int*>::value;
    bool t5 = same_t<type5, int(*)(int)>::value;
    EXPECT_TRUE(t1);
    EXPECT_TRUE(t2);
    EXPECT_TRUE(t3);
    EXPECT_TRUE(t4);
    EXPECT_TRUE(t5);
}