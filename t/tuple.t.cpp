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
  auto value = make_tuple([](int x) { return x + 1; }, 1);
  int result = apply(value);
  ASSERT_EQ(result, 2);

  // reference
  int x = 1;
  auto ref = make_tuple([&]() { x += 1; });
  apply(ref);
  ASSERT_EQ(x, 2);

  // pointer
  auto pointer = make_tuple([](int *x) { (*x) += 1; }, &x);
  apply(pointer);
  ASSERT_EQ(x, 3);
}

TEST(test_tuple, make_tuple_size_internal) {
  auto tuple = make_repeat_integer_tuple<1>();
  ASSERT_EQ(sizeof(tuple), sizeof(size_t) * 2);
  auto tuple2 = make_repeat_integer_tuple<2>();
  ASSERT_EQ(sizeof(tuple2), sizeof(size_t) * 3);
  auto tuple3 = make_repeat_integer_tuple<3>();
  ASSERT_EQ(sizeof(tuple3), sizeof(size_t) * 4);
}