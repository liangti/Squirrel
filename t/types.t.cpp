#include <gtest/gtest.h>
#include <metaprogramming/types.h>

#include <functional>
#include <memory>
#include <vector>

using namespace sqrl;

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
  bool t1 = same_t<int, sqrl::remove_cv<int>::type>::value;
  bool t2 = same_t<int, sqrl::remove_cv<const int>::type>::value;
  bool t3 = same_t<int, sqrl::remove_cv<volatile int>::type>::value;
  bool t4 = same_t<int, sqrl::remove_cv<const volatile int>::type>::value;
  EXPECT_TRUE(t1);
  EXPECT_TRUE(t2);
  EXPECT_TRUE(t3);
  EXPECT_TRUE(t4);
}

TEST(test_types, conditional_choose_types) {
  using type1 = sqrl::conditional<true, int, double>::type;
  using type2 = sqrl::conditional<false, int, double>::type;
  bool t1 = same_t<type1, int>::value;
  bool t2 = same_t<type2, double>::value;
  EXPECT_TRUE(t1);
  EXPECT_TRUE(t2);
}

TEST(test_types, remove_reference) {
  using type1 = sqrl::remove_reference<int>::type;
  using type2 = sqrl::remove_reference<int &>::type;
  using type3 = sqrl::remove_reference<int &&>::type;
  bool t1 = same_t<type1, int>::value;
  bool t2 = same_t<type2, int>::value;
  bool t3 = same_t<type3, int>::value;
  EXPECT_TRUE(t1);
  EXPECT_TRUE(t2);
  EXPECT_TRUE(t3);
}

TEST(test_types, remove_array) {
  using type1 = sqrl::remove_array<int>::type;
  using type2 = sqrl::remove_array<int[]>::type;
  using type3 = sqrl::remove_array<int[3]>::type;
  bool t1 = same_t<type1, int>::value;
  bool t2 = same_t<type2, int>::value;
  bool t3 = same_t<type3, int>::value;
  EXPECT_TRUE(t1);
  EXPECT_TRUE(t2);
  EXPECT_TRUE(t3);
}

TEST(test_types, remove_pointer) {
  using type1 = sqrl::remove_pointer<int>::type;
  using type2 = sqrl::remove_pointer<int *>::type;
  using type3 = sqrl::remove_pointer<const int *>::type;
  using type4 = sqrl::remove_pointer<int *const>::type;
  bool t1 = same_t<type1, int>::value;
  bool t2 = same_t<type2, int>::value;
  bool t3 = same_t<type3, int>::value;
  bool t4 = same_t<type4, int>::value;
  EXPECT_TRUE(t1);
  EXPECT_TRUE(t2);
  EXPECT_TRUE(t3);
  EXPECT_TRUE(t4);
}

TEST(test_types, is_pointer) {
  bool t1 = sqrl::is_pointer<int>::value;
  bool t2 = sqrl::is_pointer<int &>::value;
  bool t3 = sqrl::is_pointer<int *>::value;
  bool t4 = sqrl::is_pointer<int **>::value;
  EXPECT_FALSE(t1);
  EXPECT_FALSE(t2);
  EXPECT_TRUE(t3);
  EXPECT_TRUE(t4);
}

TEST(test_types, is_array) {
  bool t1 = sqrl::is_array<int>::value;
  bool t2 = sqrl::is_array<int[]>::value;
  bool t3 = sqrl::is_array<int[3]>::value;
  EXPECT_FALSE(t1);
  EXPECT_TRUE(t2);
  EXPECT_TRUE(t3);
}

TEST(test_types, is_function) {
  bool t1 = sqrl::is_function<int>::value;
  bool t2 = sqrl::is_function<int(int)>::value;
  EXPECT_FALSE(t1);
  EXPECT_TRUE(t2);
}

TEST(test_types, decay_types) {
  using type1 = sqrl::decay<int &>::type;
  using type2 = sqrl::decay<int &&>::type;
  using type3 = sqrl::decay<const int>::type;
  using type4 = sqrl::decay<int[]>::type;
  using type5 = sqrl::decay<int(int)>::type;
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
  // try std::reference_wrapper, not support in sqrl::reference_wrapper yet
  std::vector<int> v1{1, 2, 3};
  std::vector<int> v2{v1.begin(), v1.end()};
  std::vector<std::reference_wrapper<int>> v3{v1.begin(), v1.end()};
  v1[0] = 100;
  ASSERT_EQ(v1[0], 100);
  ASSERT_EQ(v2[0], 1);
  ASSERT_EQ(v3[0], 100);

  int i = 1;
  sqrl::reference_wrapper<int> i1(i); // lvalue
  // sqrl::reference_wrapper<int> i2(1); // rvalue is not allowed
  ASSERT_EQ(*i1, 1);
  i = 3;
  ASSERT_EQ(*i1, 3);
}

class NoThrow {
private:
  [[maybe_unused]] int data;

public:
  NoThrow() {}
  NoThrow(int data) noexcept : data(data) {}
  NoThrow(const NoThrow &copy) { throw 1; }
  NoThrow(NoThrow &&move) noexcept {}
};

TEST(test_types, is_nothrow_constructible) {
  bool a = is_nothrow_constructible<NoThrow>::value;
  bool b = is_nothrow_constructible<NoThrow, int>::value;
  bool c = is_nothrow_constructible<NoThrow, const NoThrow &>::value;
  bool d = is_nothrow_constructible<NoThrow, NoThrow &&>::value;
  EXPECT_FALSE(a);
  EXPECT_TRUE(b);
  EXPECT_FALSE(c);
  EXPECT_TRUE(d);
}

template <typename T> void test_type_identity_func(type_identity_t<T> t) {}

template <typename T>
void test_type_identity_func(T t1, type_identity_t<T> t2) {}

template <typename T> struct test_type_identity_object {
  test_type_identity_object() = delete;
  test_type_identity_object(type_identity_t<T> t) {}
};

TEST(test_types, type_identity) {

#ifdef __SQRL_TEST_EXPECT_COMPILE_TIME_FAILURE
  test_type_identity_func(1.0);     // compile error, can't infer type T
  test_type_identity_object obj(3); // compile error, can't infer
#endif
  test_type_identity_func<double>(1.0);
  test_type_identity_func(1.0, 1.0);     // can infer type T from first argument
  test_type_identity_object<int> obj(3); // compile error, can't infer
}

template <typename T, T... I>
T add_x_times(T input[], integer_sequence<T, I...> seqs) {
  T result = 0;
  ((result += input[I]), ...);
  return result;
}

TEST(test_types, integer_sequence) {
  size_t local[3] = {1, 2, 3};
  size_t result = add_x_times(local, make_index_sequence<3>{});
  ASSERT_EQ(result, 6);
}