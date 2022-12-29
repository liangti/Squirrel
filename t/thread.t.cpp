#include <gtest/gtest.h>

#include <thread/thread.h>

void *add_one_fp(void *x) {
  *(int *)x += 1;
  return x;
}

TEST(thread_wrapper, thread_create) {
  int input = 30;
  sqrl::Thread t1(add_one_fp, &input);
  t1.join();
  ASSERT_EQ(input, 31);
}

TEST(thread_wrapper, thread_raii) {
  int input = 30;
  {
    sqrl::Thread t1(add_one_fp, &input);
    // join before the end of its life-cycle
  }
  ASSERT_EQ(input, 31);
}