#include <gtest/gtest.h>
#include <metaprogramming/types.h>

#include <functional>
#include <memory>
#include <vector>

using namespace sql;

// for testing enable_if
template <typename T>
typename enable_if<is_array<T>::value, bool>::type is_array_v() {
  return true;
}

template <typename T>
typename enable_if<!is_array<T>::value, bool>::type is_array_v() {
  return false;
}

// for testing result_of
int func(int) { return 1; }

// for testing address_of
class DiffRef {
public:
  DiffRef *operator*() { return this; }
  int operator&() const { return 10; }
};

TEST(test_types, check_type_equivalent) {
  bool t1 = same_t<int, int>::value;
  bool t2 = same_t<int, double>::value;
  bool t3 = same_t<int, int *>::value;
  bool t4 = same_t<int, int &>::value;
  bool t5 = same_t<int, int &&>::value;
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

TEST(test_types, remove_const_and_volatile) {
  bool t1 = same_t<int, sql::remove_cv<int>::type>::value;
  bool t2 = same_t<int, sql::remove_cv<const int>::type>::value;
  bool t3 = same_t<int, sql::remove_cv<volatile int>::type>::value;
  bool t4 = same_t<int, sql::remove_cv<const volatile int>::type>::value;
  EXPECT_TRUE(t1);
  EXPECT_TRUE(t2);
  EXPECT_TRUE(t3);
  EXPECT_TRUE(t4);
}

TEST(test_types, conditional_choose_types) {
  using type1 = sql::conditional<true, int, double>::type;
  using type2 = sql::conditional<false, int, double>::type;
  bool t1 = same_t<type1, int>::value;
  bool t2 = same_t<type2, double>::value;
  EXPECT_TRUE(t1);
  EXPECT_TRUE(t2);
}

TEST(test_types, remove_reference) {
  using type1 = sql::remove_reference<int>::type;
  using type2 = sql::remove_reference<int &>::type;
  using type3 = sql::remove_reference<int &&>::type;
  bool t1 = same_t<type1, int>::value;
  bool t2 = same_t<type2, int>::value;
  bool t3 = same_t<type3, int>::value;
  EXPECT_TRUE(t1);
  EXPECT_TRUE(t2);
  EXPECT_TRUE(t3);
}

TEST(test_types, remove_array) {
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

TEST(test_types, is_pointer) {
  bool t1 = sql::is_pointer<int>::value;
  bool t2 = sql::is_pointer<int&>::value;
  bool t3 = sql::is_pointer<int*>::value;
  bool t4 = sql::is_pointer<int**>::value;
  EXPECT_FALSE(t1);
  EXPECT_FALSE(t2);
  EXPECT_TRUE(t3);
  EXPECT_TRUE(t4);
}

TEST(test_types, is_array) {
  bool t1 = sql::is_array<int>::value;
  bool t2 = sql::is_array<int[]>::value;
  bool t3 = sql::is_array<int[3]>::value;
  EXPECT_FALSE(t1);
  EXPECT_TRUE(t2);
  EXPECT_TRUE(t3);
}

TEST(test_types, is_function) {
  bool t1 = sql::is_function<int>::value;
  bool t2 = sql::is_function<int(int)>::value;
  EXPECT_FALSE(t1);
  EXPECT_TRUE(t2);
}

TEST(test_types, decay_types) {
  using type1 = sql::decay<int &>::type;
  using type2 = sql::decay<int &&>::type;
  using type3 = sql::decay<const int>::type;
  using type4 = sql::decay<int[]>::type;
  using type5 = sql::decay<int(int)>::type;
  bool t1 = same_t<type1, int>::value;
  bool t2 = same_t<type2, int>::value;
  bool t3 = same_t<type3, int>::value;
  bool t4 = same_t<type4, int *>::value;
  bool t5 = same_t<type5, int (*)(int)>::value;
  EXPECT_TRUE(t1);
  EXPECT_TRUE(t2);
  EXPECT_TRUE(t3);
  EXPECT_TRUE(t4);
  EXPECT_TRUE(t5);
}

TEST(test_types, enable_if) {
  bool t1 = is_array_v<int>();
  bool t2 = is_array_v<int[]>();
  bool t3 = is_array_v<int[3]>();
  EXPECT_FALSE(t1);
  EXPECT_TRUE(t2);
  EXPECT_TRUE(t3);
}

TEST(test_types, result_of) {
  // not support overload since decltype<f> is ambiguous if f has overload
  // copies
  auto f = [](int a) -> int { return a + 1; };
  using type1 = result_of<decltype(f)(int)>::type;
  using type2 = result_of<decltype (&func)(int)>::type;
  bool t1 = same_t<type1, int>::value;
  bool t2 = same_t<type2, int>::value;
  EXPECT_TRUE(t1);
  EXPECT_TRUE(t2);
}

TEST(test_types, address_of) {
  DiffRef dr;
  const DiffRef &dr2 = dr;
  ASSERT_EQ(&dr, 10);
  ASSERT_EQ(*dr, address_of(dr));
  ASSERT_EQ(&dr2, 10);
  ASSERT_EQ(*dr, address_of(dr2)); // dr dr2 should have same address
}

TEST(test_types, reference_wrapper) {
  // try std::reference_wrapper, not support in sql::reference_wrapper yet
  std::vector<int> v1{1, 2, 3};
  std::vector<int> v2{v1.begin(), v1.end()};
  std::vector<std::reference_wrapper<int>> v3{v1.begin(), v1.end()};
  v1[0] = 100;
  ASSERT_EQ(v1[0], 100);
  ASSERT_EQ(v2[0], 1);
  ASSERT_EQ(v3[0], 100);

  int i = 1;
  sql::reference_wrapper<int> i1(i); // lvalue
  // sql::reference_wrapper<int> i2(1); // rvalue is not allowed
  ASSERT_EQ(*i1, 1);
  i = 3;
  ASSERT_EQ(*i1, 3);
}