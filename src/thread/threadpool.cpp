#include <thread/threadpool.h>

namespace sql {

ThreadPool::ThreadPool(int pool_size) : pool_size(pool_size) {
  handled_tasks_num = 0;
}

void ThreadPool::start() {
  begin = true;
  workers.reserve(pool_size);
  for (int i = 0; i < pool_size; i++) {
    workers.emplace_back(new std::thread([this]() {
      while (true) {
        Task task;
        // critical area, need lock for thread safe
        {
          std::unique_lock<std::mutex> guard(lock);
          // release the lock and wait till condition meet
          this->condition.wait(
              guard, [this]() { return !this->begin || !this->tasks.empty(); });
          // finished waiting and acquire the lock again
          if (!this->begin && this->tasks.empty()) {
            return;
          }
          if (this->tasks.empty()) {
            guard.unlock();
            continue;
          }
          task = this->tasks.front();
          this->tasks.pop();
          this->handled_tasks_num++;
          task();
        }
      }
    }));
  }
}

void ThreadPool::stop() {
  {
    std::unique_lock<std::mutex> guard(lock);
    begin = false;
  }
  condition.notify_all();
  for (auto itr = workers.cbegin(); itr != workers.cend(); itr++) {
    (*itr)->join();
    delete (*itr);
  }
  workers.clear();
}

}; // namespace sql