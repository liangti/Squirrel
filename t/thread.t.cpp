#include <gtest/gtest.h>

#include <thread/thread.h>

void add(int *x, int *y, int *sum) { (*sum) = (*x) + (*y); }

TEST(thread_wrapper, thread_create) {
  int x = 30;
  int y = 40;
  int sum = 0;
  sqrl::Thread t1(add, &x, &y, &sum);
  t1.join();
  ASSERT_EQ(x, 30);
  ASSERT_EQ(y, 40);
  ASSERT_EQ(sum, 70);
}

TEST(thread_wrapper, thread_raii) {
  int x = 30;
  int y = 40;
  int sum = 0;
  {
    sqrl::Thread t1(add, &x, &y, &sum);
    // join before the end of its life-cycle
  }
  ASSERT_EQ(x, 30);
  ASSERT_EQ(y, 40);
  ASSERT_EQ(sum, 70);
}