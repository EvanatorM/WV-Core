#include <wv/threading/ThreadPool.h>

#include <wv/Logger.h>

namespace WillowVox
{
    void ThreadPool::Start(int numThreads)
    {
        for (int i = 0; i < numThreads; i++)
            m_threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
    }

    void ThreadPool::QueueJob(const std::function<void()>& job, bool highPriority)
    {
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            if (highPriority)
                m_highPriorityJobs.push(job);
            else
                m_lowPriorityJobs.push(job);
        }
        m_mutexCondition.notify_one();
    }

    void ThreadPool::Stop()
    {
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_shouldTerminate = true;
        }
        m_mutexCondition.notify_all();
        for (std::thread& activeThread : m_threads)
            activeThread.join();
        
        m_threads.clear();
    }

    void ThreadPool::ThreadLoop()
    {
        while (true)
        {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_mutexCondition.wait(lock, [this] {
                    return !m_highPriorityJobs.empty() || !m_lowPriorityJobs.empty() || m_shouldTerminate;
                });
                if (m_shouldTerminate)
                    return;
                if (!m_highPriorityJobs.empty())
                {
                    job = m_highPriorityJobs.front();
                    m_highPriorityJobs.pop();
                }
                else
                {
                    job = m_lowPriorityJobs.front();
                    m_lowPriorityJobs.pop();
                }
            }
            job();
        }
    }
}