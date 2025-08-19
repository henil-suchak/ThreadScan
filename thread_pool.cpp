#include "thread_pool.hpp"
#include "thread_worker.hpp"
#include <unistd.h> // for usleep

// --- TaskQueue Implementation ---
TaskQueue::TaskQueue() : stop_flag(false) {
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond, nullptr);
}

TaskQueue::~TaskQueue() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

void TaskQueue::push(const ThreadArgs& task) {
    pthread_mutex_lock(&mutex);
    queue.push(task);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

bool TaskQueue::pop(ThreadArgs& task) {
    pthread_mutex_lock(&mutex);
    while (queue.empty() && !stop_flag) {
        pthread_cond_wait(&cond, &mutex);
    }
    
    if (stop_flag && queue.empty()) {
        pthread_mutex_unlock(&mutex);
        return false;
    }

    task = queue.front();
    queue.pop();
    pthread_mutex_unlock(&mutex);
    return true;
}

void TaskQueue::signal_stop() {
    pthread_mutex_lock(&mutex);
    stop_flag = true;
    pthread_cond_broadcast(&cond); // Wake up all waiting threads
    pthread_mutex_unlock(&mutex);
}

bool TaskQueue::is_empty() {
    pthread_mutex_lock(&mutex);
    bool empty = queue.empty();
    pthread_mutex_unlock(&mutex);
    return empty;
}

// --- ThreadPool Implementation ---
ThreadPool::ThreadPool(size_t num_threads) {
    for (size_t i = 0; i < num_threads; ++i) {
        pthread_t thread;
        pthread_create(&thread, nullptr, worker_thread_function, this);
        workers.push_back(thread);
    }
}

ThreadPool::~ThreadPool() {
    task_queue.signal_stop();
    for (pthread_t& worker : workers) {
        pthread_join(worker, nullptr);
    }
}

void ThreadPool::add_task(const ThreadArgs& task) {
    task_queue.push(task);
}

void* ThreadPool::worker_thread_function(void* arg) {
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    ThreadArgs task;
    
    while (pool->task_queue.pop(task)) {
        process_chunk_dispatcher(&task);
    }
    return nullptr;
}

void ThreadPool::wait_for_completion() {
    while (!task_queue.is_empty()) {
        usleep(10000); // 10ms
    }
}