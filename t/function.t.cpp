#include <functional/function.h>
#include <gtest/gtest.h>

using namespace sqrl;

void add_one(int *i) { (*i)++; }

struct Functor {
  void operator()(int *i) { (*i)++; }
};

TEST(function_test, release_empty_callable) {
  { [[maybe_unused]] Function<void(int *)> fp; }
  { [[maybe_unused]] Function<void()> fp; }
}

TEST(function_test, function_pointer) {
  Function<void(int *)> fp;
  fp = add_one;
  int local = 3;
  fp(&local);
  EXPECT_EQ(local, 4);
}

TEST(function_test, lambda) {
  Function<void()> fp;
  int local = 3;
  fp = [&]() { local = 4; };
  fp();
  EXPECT_EQ(local, 4);
}

// this is what function pointer can't handle
TEST(function_test, functor) {
  Function<void(int *)> fp;
  Functor obj;
  fp = obj;
  int local = 3;
  fp(&local);
  EXPECT_EQ(local, 4);
}

TEST(function_test, copy) {
  Function<void(int *)> fp(add_one);
  auto copy_fp = fp;
  int local = 3;
  copy_fp(&local);
  EXPECT_EQ(local, 4);
}

TEST(function_test, move) {
  Function<void(int *)> fp(add_one);
  auto move_fp = std::move(fp);
  int local = 3;
  move_fp(&local);
  EXPECT_EQ(local, 4);
}