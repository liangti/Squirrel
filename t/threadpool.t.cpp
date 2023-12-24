#include <gtest/gtest.h>
#include <thread/threadpool.h>

using namespace sqrl;

static const int MAX_LOOP_NUM = 10;

class ThreadPoolTest : public ::testing::TestWithParam<int> {};

TEST_P(ThreadPoolTest, no_task) {
  ThreadPool tp(4);
  tp.start();
  tp.stop();
  ASSERT_EQ(tp.get_handled_tasks_num(), 0);
  ASSERT_EQ(tp.get_pool_size(), 4);
}

TEST_P(ThreadPoolTest, simple_add_task) {
  ThreadPool tp;
  tp.start();
  tp.enqueue([]() {});
  tp.stop();
  ASSERT_EQ(tp.get_handled_tasks_num(), 1);
}

TEST_P(ThreadPoolTest, add_task_change_local_var) {
  ThreadPool tp;
  int *local = new int(1);
  tp.start();
  tp.enqueue([local]() {
    *local = 3;
    return 0;
  });
  tp.stop();
  ASSERT_EQ(*local, 3);
}

TEST_P(ThreadPoolTest, add_tasks_more_than_pool_size) {
  auto func = []() {};
  ThreadPool tp;
  int tasks_num = tp.get_pool_size() + 1;
  tp.start();
  for (int i = 0; i < tasks_num; i++) {
    tp.enqueue(func);
  }
  tp.stop();
  ASSERT_EQ(tp.get_handled_tasks_num(), tasks_num);
}

TEST_P(ThreadPoolTest, add_task_with_parameters) {
  bool flag = false;
  auto func = [&flag](int a) -> int {
    flag = true;
    return a + 1;
  };
  ThreadPool tp;
  tp.start();
  EXPECT_FALSE(flag);
  auto result = tp.enqueue(func, 1);
  tp.stop();
  EXPECT_TRUE(flag);
  ASSERT_EQ(tp.get_handled_tasks_num(), 1);
  ASSERT_EQ(result.get(), 2);
}

INSTANTIATE_TEST_SUITE_P(test_threadpool, ThreadPoolTest,
                         ::testing::Range(1, MAX_LOOP_NUM));