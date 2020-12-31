#include <gtest/gtest.h>
#include <thread>
#include <container/safe_queue.h>

using namespace sql;

static const int MAX_LOOP_NUM = 10;

class SafeQueueTest: public ::testing::TestWithParam<int> {};

TEST_P(SafeQueueTest, simple_producer){
    SafeQueue<int> sq(1000);
    auto producer = [&sq](size_t test_size){
        for(size_t i = 0; i < test_size; i++){
            if(i % 2 == 0){
                sq.push(i);
            }
            else{
                sq.emplace(i);
            }
        }
    };
    std::thread t1(producer, 200);
    std::thread t2(producer, 200);
    std::thread t3(producer, 200);
    std::thread t4(producer, 200);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    ASSERT_EQ(sq.size(), 800);
}

TEST_P(SafeQueueTest, simple_consumer){
    size_t test_size = 1000;
    SafeQueue<int> sq(test_size);
    for(size_t i = 0; i < test_size; i++){
        sq.push(i);
    }
    ASSERT_EQ(sq.size(), test_size);
    auto consumer = [&sq](size_t batch_size){
        for(size_t i = 0; i < batch_size; i++){
            sq.pop();
        }
    };
    std::thread t1(consumer, 200);
    std::thread t2(consumer, 200);
    std::thread t3(consumer, 200);
    std::thread t4(consumer, 200);
    std::thread t5(consumer, 200);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    ASSERT_EQ(sq.size(), 0);
}

INSTANTIATE_TEST_SUITE_P(test_safe_queue, SafeQueueTest, ::testing::Range(1, MAX_LOOP_NUM));