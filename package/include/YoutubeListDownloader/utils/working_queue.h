// Copyright (c) 2018 Linus Klöckner

#ifndef INCLUDE_UTILS_WORKING_QUEUE_H_
#define INCLUDE_UTILS_WORKING_QUEUE_H_

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace Utils {

    class WorkingQueue {
     public:
        explicit WorkingQueue(bool p_start = true,
                              int p_nr = std::thread::hardware_concurrency(),
                              const std::function<void(size_t)>& p_f = [](size_t) {});
        ~WorkingQueue();

        void AddTask(const std::function<void()>& p_f);
        void Start();

     private:
        using Lock = std::unique_lock<std::mutex>;
        using MS = std::chrono::duration<int, std::milli>;

        void AddWorker();

        std::vector<std::thread> m_thread_pool;
        bool m_stop;
        bool m_wait;
        std::function<void(size_t)> m_progress;

        std::mutex m_mutex_access;
        std::mutex m_mutex_finish;
        std::condition_variable m_cond_add;
        std::deque<std::function<void()>> m_queue_tasks;
    };

}   // namespace Utils

#endif   // INCLUDE_UTILS_WORKING_QUEUE_H_
