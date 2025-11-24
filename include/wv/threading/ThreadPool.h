#pragma once

#include <wv/wvpch.h>

namespace WillowVox
{
    class ThreadPool
    {
    public:
        void Start(int numThreads);
        void QueueJob(const std::function<void()>& job, bool highPriority = false);
        void Stop();

    private:
        void ThreadLoop();

        bool m_shouldTerminate = false;
        std::mutex m_queueMutex;
        std::condition_variable m_mutexCondition;
        std::vector<std::thread> m_threads;
        std::queue<std::function<void()>> m_lowPriorityJobs;
        std::queue<std::function<void()>> m_highPriorityJobs;
    };
}