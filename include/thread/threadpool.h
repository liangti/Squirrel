#ifndef INCLUDED_THREADPOOL_H
#define INCLUDED_THREADPOOL_H

#include <condition_variable>
#include <mutex>
#include <utility>

#include <async/future.h>
#include <container/queue.h>
#include <container/vector.h>
#include <functional/function.h>
#include <metaprogramming/types.h>
#include <thread/thread.h>

namespace sqrl {

class ThreadPool {
public:
  ThreadPool(int pool_size = 3);
  typedef sqrl::Function<void()> Task;
  void start();
  void stop();

  template <class T, class... Args>
  auto enqueue(T &&t, Args &&...args)
      -> sqrl::Future<typename sqrl::result_of<T(Args...)>::type>;

  // remove copy methods
  ThreadPool(const ThreadPool &) = delete;
  const ThreadPool &operator=(const ThreadPool &) = delete;

  // hooks
  inline int get_handled_tasks_num() { return handled_tasks_num; }
  inline int get_pool_size() { return pool_size; }

private:
  typedef sqrl::Vector<sqrl::Thread *> Threads;
  Threads workers;
  sqrl::Queue<Task> tasks;
  std::mutex lock;
  std::condition_variable condition;
  bool begin;
  int handled_tasks_num;
  int pool_size;
};
template <class T, class... Args>
auto ThreadPool::enqueue(T &&t, Args &&...args)
    -> sqrl::Future<typename sqrl::result_of<T(Args...)>::type> {
  using return_type = typename sqrl::result_of<T(Args...)>::type;

  auto task = std::make_shared<sqrl::PackageTask<return_type()>>(
      std::bind(std::forward<T>(t), std::forward<Args>(args)...));
  sqrl::Future<return_type> result = task->get_future();
  {
    std::unique_lock<std::mutex> guard(lock);
    tasks.emplace([task]() { (*task)(); });
  }
  condition.notify_one();
  return result;
}

}; // namespace sqrl

#endif