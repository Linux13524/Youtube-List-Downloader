// Copyright 2018 Linus Klöckner

#include "YoutubeListDownloader/utils/working_queue.h"

#include <utility>

Utils::WorkingQueue::WorkingQueue(bool p_start, int p_nr, const std::function<void(size_t)>& p_f)
    : m_stop(false)
    , m_wait(!p_start)
    , m_progress(p_f) {
    // Add workers
    while (p_nr-- > 0) {
        AddWorker();
    }
}

Utils::WorkingQueue::~WorkingQueue() {
    Lock lock_stop{m_mutex_finish};

    // Wait for all tasks to be finished
    while (!m_queue_tasks.empty())
        std::this_thread::sleep_for(MS(100));

    // Notify workers to stop
    m_stop = true;

    // Wait for workers to stop
    for (std::thread& thread : m_thread_pool)
        thread.join();

    m_progress(0);

    m_thread_pool.clear();
}

void Utils::WorkingQueue::AddTask(const std::function<void()>& p_f) {
    Lock lock1(m_mutex_access);
    Lock lock2(m_mutex_finish);

    if (m_stop)
        return;

    // Add task to queue
    m_queue_tasks.push_back(p_f);

    m_cond_add.notify_one();
}

void Utils::WorkingQueue::Start() {
    m_wait = false;
}

void Utils::WorkingQueue::AddWorker() {
    // Create worker
    std::thread t([this]() {
        while (!m_stop) {
            std::function<void()> task;
            {
                Lock lock{m_mutex_access};
                // Waiting for job
                if (m_queue_tasks.empty() || m_wait) {
                    m_cond_add.wait_for(lock, MS(5));
                    continue;
                }
                // Get task and remove from queue
                m_progress(m_queue_tasks.size());
                task = std::move(m_queue_tasks.front());
                m_queue_tasks.pop_front();
            }
            // Run task
            task();
        }
    });
    // Add worker to pool
    m_thread_pool.push_back(std::move(t));
}
