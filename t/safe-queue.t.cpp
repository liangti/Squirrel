#include <container/safe_queue.h>
#include <gtest/gtest.h>
#include <thread>

using namespace sqrl;

static const int MAX_LOOP_NUM = 10; // TODO: raise the loop number to 100

class SafeQueueTest : public ::testing::TestWithParam<int> {};
class BlockFreeQueueTest : public ::testing::TestWithParam<int> {};

template <template <class> class Q, typename D = int>
void test_producer_simple() {
  Q<D> sq(1000);
  auto producer = [&sq](size_t test_size) {
    for (size_t i = 0; i < test_size; i++) {
      if (i % 2 == 0) {
        sq.push(i);
      } else {
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

template <template <class> class Q, typename D = int>
void test_consumer_simple() {
  size_t test_size = 1000;
  Q<D> sq(test_size);
  for (size_t i = 0; i < test_size; i++) {
    sq.push(i);
  }
  ASSERT_EQ(sq.size(), test_size);
  auto consumer = [&sq](size_t batch_size) {
    for (size_t i = 0; i < batch_size; i++) {
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

template <template <class> class Q, typename D = int>
void test_queue_read_write() {
  size_t test_size = 1000;
  Q<D> sq(test_size);

  auto producer = [&sq](size_t batch_size) {
    for (size_t i = 0; i < batch_size; i++) {
      if (i % 2 == 0) {
        sq.push(i);
      } else {
        sq.emplace(i);
      }
    }
  };
  auto consumer = [&sq](size_t batch_size) {
    for (size_t i = 0; i < batch_size; i++) {
      sq.pop();
    }
  };
  std::thread t1(producer, 200);
  std::thread t2(consumer, 200);
  std::thread t3(producer, 200);
  std::thread t4(consumer, 200);
  std::thread t5(producer, 200);
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  t5.join();
  ASSERT_EQ(sq.size(), 200);
}

TEST_P(SafeQueueTest, simple_producer) { test_producer_simple<SafeQueue>(); }

TEST_P(SafeQueueTest, simple_consumer) { test_consumer_simple<SafeQueue>(); }

TEST_P(SafeQueueTest, read_write) { test_queue_read_write<SafeQueue>(); }

TEST_P(BlockFreeQueueTest, simple_producer) {
  test_producer_simple<BlockFreeQueue>();
}

TEST_P(BlockFreeQueueTest, simple_consumer) {
  test_consumer_simple<BlockFreeQueue>();
}

TEST_P(BlockFreeQueueTest, read_write) {
  test_queue_read_write<BlockFreeQueue>();
}

INSTANTIATE_TEST_SUITE_P(test_safe_queue, SafeQueueTest,
                         ::testing::Range(1, MAX_LOOP_NUM));
INSTANTIATE_TEST_SUITE_P(test_block_free_queue, BlockFreeQueueTest,
                         ::testing::Range(1, MAX_LOOP_NUM));