#ifndef INCLUDED_THREAD_H
#define INCLUDED_THREAD_H

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include <metaprogramming/types.h>

namespace sqrl {

class ThreadPool {
public:
  ThreadPool(int pool_size = 3);
  typedef std::function<void()> Task;
  void start();
  void stop();

  template <class T, class... Args>
  auto enqueue(T &&t, Args &&...args)
      -> std::future<typename sqrl::result_of<T(Args...)>::type>;

  // remove copy methods
  ThreadPool(const ThreadPool &) = delete;
  const ThreadPool &operator=(const ThreadPool &) = delete;

  // hooks
  inline int get_handled_tasks_num() { return handled_tasks_num; }
  inline int get_pool_size() { return pool_size; }

private:
  typedef std::vector<std::thread *> Threads;
  Threads workers;
  std::queue<Task> tasks;
  std::mutex lock;
  std::condition_variable condition;
  bool begin;
  int handled_tasks_num;
  int pool_size;
};
template <class T, class... Args>
auto ThreadPool::enqueue(T &&t, Args &&...args)
    -> std::future<typename sqrl::result_of<T(Args...)>::type> {
  using return_type = typename sqrl::result_of<T(Args...)>::type;

  auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<T>(t), std::forward<Args>(args)...));
  std::future<return_type> result = task->get_future();
  {
    std::unique_lock<std::mutex> guard(lock);
    tasks.emplace([task]() { (*task)(); });
  }
  condition.notify_one();
  return result;
}

}; // namespace sqrl

#endif