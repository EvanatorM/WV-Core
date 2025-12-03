#pragma once

#include <concurrentqueue.h>
#include <blockingconcurrentqueue.h>
#include <wv/wvpch.h>
#include <atomic>
#include <array>

namespace WillowVox
{
    enum class Priority {
        High = 0,
        Medium = 1,
        Low = 2,
        Count = 3
    };

    class ThreadPool
    {
    public:
        ThreadPool();
        ~ThreadPool();
        void Start(int numThreads);
        void Enqueue(const std::function<void()>& job, Priority priority = Priority::Medium);

    private:
        void ThreadLoop();

        // Queues for each priority
        std::array<moodycamel::ConcurrentQueue<std::function<void()>>, static_cast<int>(Priority::Count)> m_queues;
        // Queue to wake up threads when a job is queued
        moodycamel::BlockingConcurrentQueue<bool> m_signal;

        std::vector<std::thread> m_threads;
        std::atomic<bool> m_shouldTerminate;
    };
}