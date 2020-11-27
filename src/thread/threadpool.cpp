#include <thread/threadpool.h>

namespace sql{

ThreadPool::ThreadPool(){
    handled_tasks_num = 0;
}

void ThreadPool::start(){
    begin = true;
    workers.reserve(thread_pool_size);
    for(int i = 0; i < thread_pool_size; i++){
        workers.emplace_back(new std::thread([this](){
            while(true){
                Task task;
                // critical area, need lock for thread safe
                {
                    std::unique_lock<std::mutex> guard(lock);
                    if(!this->begin && this->tasks.empty()){
                        guard.unlock();
                        return;
                    }
                    if(this->tasks.empty()){
                        guard.unlock();
                        continue;
                    }
                    task = this->tasks.front();
                    this->tasks.pop();
                    this->handled_tasks_num++;
                    guard.unlock();
                }
                // task();
            }
        }));
    }
}

void ThreadPool::enqueue(const Task& task){
    // critical area, need lock for thread safe
    {
        std::unique_lock<std::mutex> guard(lock);
        tasks.emplace(task);
        guard.unlock();
    }
}

void ThreadPool::stop(){
    {
        std::unique_lock<std::mutex> guard(lock);
        begin = false;
        guard.unlock();
    }
    for(auto itr = workers.cbegin(); itr != workers.cend(); itr++){
        (*itr)->join();
        delete (*itr);
    }
    workers.clear();
}

}; // namespace sql