#include <gtest/gtest.h>

#include <thread/thread.h>

void add(int *x, int *y, int *sum) { (*sum) = (*x) + (*y); }

TEST(thread_wrapper, thread_create_and_join) {
  int x = 30;
  int y = 40;
  int sum = 0;
  sqrl::Thread t1(add, &x, &y, &sum);
  ASSERT_TRUE(t1.joinable());
  t1.join();
  ASSERT_FALSE(t1.joinable());
  EXPECT_EQ(x, 30);
  EXPECT_EQ(y, 40);
  EXPECT_EQ(sum, 70);
}

TEST(thread_wrapper, thread_raii) {
  int x = 30;
  int y = 40;
  int sum = 0;
  {
    sqrl::Thread t1(add, &x, &y, &sum);
    t1.join();
    // will not join before the end of its life-cycle
  }
  EXPECT_EQ(x, 30);
  EXPECT_EQ(y, 40);
  EXPECT_EQ(sum, 70);
}

TEST(thread_wrapper, jthread_create_and_join) {
  int x = 30;
  int y = 40;
  int sum = 0;
  sqrl::JThread t1(add, &x, &y, &sum);
  ASSERT_TRUE(t1.joinable());
  t1.join();
  ASSERT_FALSE(t1.joinable());
  EXPECT_EQ(x, 30);
  EXPECT_EQ(y, 40);
  EXPECT_EQ(sum, 70);
}

TEST(thread_wrapper, jthread_raii) {
  int x = 30;
  int y = 40;
  int sum = 0;
  {
    sqrl::JThread t1(add, &x, &y, &sum);
    // join before the end of its life-cycle
  }
  EXPECT_EQ(x, 30);
  EXPECT_EQ(y, 40);
  EXPECT_EQ(sum, 70);
}