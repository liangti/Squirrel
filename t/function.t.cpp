#include <functional/function.h>
#include <gtest/gtest.h>

using namespace sql;

void add_one(int *i) { (*i)++; }

struct Functor {
  void operator()(int *i) { (*i)++; }
};

TEST(function_test, release_empty_callable) {
  { function<void(int *)> fp; }
}

TEST(function_test, function_pointer) {
  function<void(int *)> fp;
  fp = add_one;
  int local = 3;
  fp(&local);
  ASSERT_EQ(local, 4);
}

TEST(function_test, lambda) {
  auto f = [](int *i) { (*i)++; };
  function<void(int *)> fp;
  fp = add_one;
  int local = 3;
  fp(&local);
  ASSERT_EQ(local, 4);
}

// this is what function pointer can't handle
TEST(function_test, functor) {
  function<void(int *)> fp;
  Functor obj;
  fp = obj;
  int local = 3;
  fp(&local);
  ASSERT_EQ(local, 4);
}