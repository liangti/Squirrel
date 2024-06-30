#include <async/future.h>
#include <gtest/gtest.h>

#include <thread>

using namespace sqrl;

TEST(future_test_basic, all_movable) {
  Promise<int> promise1;
  Promise<int> promise2 = std::move(promise1);
  Future<int> future1 = promise2.get_future();
  Future<int> future2 = std::move(future1);
  EXPECT_FALSE(future1.valid());
  EXPECT_TRUE(future2.valid());
  EXPECT_FALSE(future1.ready());
  EXPECT_FALSE(future2.ready());
#ifdef __SQRL_TEST_EXPECT_COMPILE_TIME_FAILURE
  promise.set_value();
#endif
}

TEST(future_test_basic, get_future_more_than_once_will_fail) {
  Promise<int> promise;
  Future<int> future1 = promise.get_future();
  Future<int> future2 = promise.get_future();
  EXPECT_TRUE(future1.valid());
  EXPECT_FALSE(future2.valid());
  EXPECT_FALSE(future1.ready());
  EXPECT_FALSE(future2.ready());
}

TEST(future_test_base, promise_move_get_future_state) {
  Promise<int> promise;
  Future<int> future1 = promise.get_future();
  // promise2 should have same `future_already_retrieved` state as promise
  Promise<int> promise2 = std::move(promise);
  Future<int> future2 = promise2.get_future();
  EXPECT_TRUE(future1.valid());
  EXPECT_FALSE(future2.valid());
  EXPECT_FALSE(future1.ready());
  EXPECT_FALSE(future2.ready());
}

TEST(future_test_basic, void) {
  Promise<void> promise;
  Future<void> future = promise.get_future();
  promise.set_value();
  EXPECT_TRUE(future.ready());
#ifdef __SQRL_TEST_EXPECT_COMPILE_TIME_FAILURE
  promise.set_value(void);
#endif
}

TEST(future_test_sync, basic) {
  Promise<int> promise;
  Future<int> future = promise.get_future();
  promise.set_value(100);
  EXPECT_TRUE(future.ready());
  EXPECT_EQ(future.get(), 100);
}

TEST(future_test_async, basic) {
  Promise<int> promise;
  Future<int> future = promise.get_future();
  std::thread task([&promise]() { promise.set_value(100); });
  task.join();
  EXPECT_TRUE(future.ready());
  EXPECT_EQ(future.get(), 100);
}

int add(int a, int b) { return a + b; }

TEST(package_task_test_sync, basic) {
  PackageTask<int(int, int)> scheduler(add);
  Future<int> future = scheduler.get_future();
  scheduler(1, 2);
  EXPECT_TRUE(future.ready());
  EXPECT_EQ(future.get(), 3);
}

TEST(package_task_test_async, basic) {
  PackageTask<int(int, int)> scheduler(add);
  Future<int> future = scheduler.get_future();
  std::thread task(std::move(scheduler), 1, 2);
  task.join();
  EXPECT_TRUE(future.ready());
  EXPECT_EQ(future.get(), 3);
}

TEST(package_task_test_sync, void) {
  PackageTask<void()> scheduler([]() {});
  Future<void> future = scheduler.get_future();
  scheduler();
  EXPECT_TRUE(future.ready());
}

TEST(package_task_test_async, void) {
  PackageTask<void()> scheduler([]() {});
  Future<void> future = scheduler.get_future();
  std::thread task(std::move(scheduler));
  task.join();
  EXPECT_TRUE(future.ready());
}