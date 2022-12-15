#include <gtest/gtest.h>
#include <metaprogramming/tuple.h>

using namespace sqrl;

TEST(test_tuple, construct_empty_tuple) { [[maybe_unused]] Tuple<> empty; }

TEST(test_tuple, construct_simple_tuple) {
  Tuple<char, int, double> tuple('a', 1, 0.3);
}

TEST(test_tuple, get_item_from_tuple) {
  Tuple<char, int, double> tuple('a', 1, 0.3);
  ASSERT_EQ(get<0>(tuple), 'a');
  ASSERT_EQ(get<1>(tuple), 1);
  ASSERT_EQ(get<2>(tuple), 0.3);
}

TEST(test_tuple, tuple_head_and_tail) {
  Tuple<char, int, double> tuple('a', 1, 0.3);
  ASSERT_EQ(tuple.head, 'a');
  ASSERT_EQ(tuple.tail.head, 1);
  ASSERT_EQ(tuple.tail.tail.head, 0.3);
}

TEST(test_tuple, tuple_size) {
  Tuple<char, int, double> tuple('a', 1, 0.3);
  ASSERT_EQ(size(tuple), 3);
}

TEST(test_tuple, make_tuple) {
  int a = 3;
  auto tuple = make_tuple('a', 1, a);
  a = 4;
  ASSERT_EQ(get<0>(tuple), 'a');
  ASSERT_EQ(get<1>(tuple), 1);
  ASSERT_EQ(get<2>(tuple), 3);
}

TEST(test_tuple, other_type_copy) {
  int a = 1;
  Tuple<int &> t1(a);
  Tuple<int> t2(3);
  t1 = t2;
  ASSERT_EQ(a, 3);
}

TEST(test_tuple, tie) {
  auto tuple = make_tuple(1, 2, 3);
  int a;
  int b;
  int c;
  tie(a, b, c) = tuple;
  ASSERT_EQ(a, 1);
  ASSERT_EQ(b, 2);
  ASSERT_EQ(c, 3);
}

TEST(test_tuple, ignore) {
  auto tuple = make_tuple(100, 2, 0);
  int a;
  tie(a, ignore, ignore) = tuple;
  ASSERT_EQ(a, 100);
}

TEST(test_tuple, apply) {
  auto tuple = make_tuple([](int x) { return x + 1; }, 1);
  int result = apply(tuple);
  ASSERT_EQ(result, 2);
}