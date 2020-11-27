#ifndef INCLUDED_THREAD_H
#define INCLUDED_THREAD_H

#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

namespace sql{

class ThreadPool{
public:
    ThreadPool(int pool_size=3);
    typedef std::function<void()> Task;
    void start();
    void stop();
    void enqueue(const Task&);

    // remove copy methods
    ThreadPool(const ThreadPool&)=delete;
    const ThreadPool& operator=(const ThreadPool&)=delete;

    // hooks
    inline int get_handled_tasks_num(){
        return handled_tasks_num;
    }
    inline int get_pool_size(){
        return pool_size;
    }

private:
    typedef std::vector<std::thread*> Threads;
    Threads workers;
    std::queue<Task> tasks;
    std::mutex lock;
    std::condition_variable condition;
    bool begin;
    int handled_tasks_num;
    int pool_size;
};

}; // namespace sql

#endif