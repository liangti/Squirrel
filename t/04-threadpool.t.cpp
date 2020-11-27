#include <gtest/gtest.h>
#include <thread/threadpool.h>

using namespace sql;

class ThreadPoolTest: public ::testing::TestWithParam<int> {};

TEST_P(ThreadPoolTest, no_task){
    ThreadPool tp;
    tp.start();
    tp.stop();
    ASSERT_EQ(tp.get_handled_tasks_num(), 0);
}

TEST_P(ThreadPoolTest, simple_add_task){
    ThreadPool tp;
    tp.start();
    tp.enqueue([](){return 0;});
    tp.stop();
    ASSERT_EQ(tp.get_handled_tasks_num(), 1);
}

INSTANTIATE_TEST_CASE_P(test_threadpool, ThreadPoolTest, ::testing::Range(1, 10));