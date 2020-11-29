#include <gtest/gtest.h>
#include <metaprogramming/tuple.h>

using namespace sql;

TEST(test_tuple, construct_empty_tuple){
    Tuple<> empty;
}

TEST(test_tuple, construct_simple_tuple){
    Tuple<char, int, double> tuple('a', 1, 0.3);
}

TEST(test_tuple, get_item_from_tuple){
    Tuple<char, int, double> tuple('a', 1, 0.3);
    ASSERT_EQ(get<0>(tuple), 'a');
    ASSERT_EQ(get<1>(tuple), 1);
    ASSERT_EQ(get<2>(tuple), 0.3);
}

TEST(test_tuple, tuple_head_and_tail){
    Tuple<char, int, double> tuple('a', 1, 0.3);
    ASSERT_EQ(tuple.head, 'a');
    ASSERT_EQ(tuple.tail.head, 1);
    ASSERT_EQ(tuple.tail.tail.head, 0.3);
}

TEST(test_tuple, tuple_size){
    Tuple<char, int, double> tuple('a', 1, 0.3);
    ASSERT_EQ(size(tuple), 3);
}