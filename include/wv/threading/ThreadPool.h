#pragma once

#include <concurrentqueue.h>
#include <wv/wvpch.h>
#include <atomic>

namespace WillowVox
{
    class ThreadPool
    {
    public:
        ThreadPool(int initialJobQueueCapacity);
        ThreadPool() = default;
        void Start(int numThreads);
        void QueueJob(const std::function<void()>& job, bool highPriority = false);
        void Stop();

    private:
        void ThreadLoop();

        std::atomic<bool> m_shouldTerminate;
        std::condition_variable m_mutexCondition;
        std::vector<std::thread> m_threads;
        moodycamel::ConcurrentQueue<std::function<void()>> m_lowPriorityJobs;
        moodycamel::ConcurrentQueue<std::function<void()>> m_highPriorityJobs;
    };
}