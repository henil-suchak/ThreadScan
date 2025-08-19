#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <vector>
#include <queue>
#include <pthread.h>
#include "common.hpp" // For ThreadArgs

class TaskQueue {
public:
    TaskQueue();
    ~TaskQueue();
    void push(const ThreadArgs& task);
    bool pop(ThreadArgs& task);
    void signal_stop();
    bool is_empty();

private:
    std::queue<ThreadArgs> queue;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool stop_flag;
};

class ThreadPool {
public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();
    void add_task(const ThreadArgs& task);
    void wait_for_completion();

private:
    static void* worker_thread_function(void* arg);
    
    std::vector<pthread_t> workers;
    TaskQueue task_queue;
};

#endif // THREAD_POOL_HPP