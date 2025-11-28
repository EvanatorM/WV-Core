#include <wv/threading/ThreadPool.h>

#include <wv/Logger.h>

namespace WillowVox
{
    ThreadPool::ThreadPool(int initialJobQueueCapacity = 0)
        : m_shouldTerminate(false), m_highPriorityJobs(initialJobQueueCapacity), m_lowPriorityJobs(initialJobQueueCapacity) {}

    void ThreadPool::Start(int numThreads)
    {
        for (int i = 0; i < numThreads; i++)
            m_threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
    }

    void ThreadPool::QueueJob(const std::function<void()>& job, bool highPriority)
    {
        {
            if (highPriority)
                m_highPriorityJobs.enqueue(job);
            else
                m_lowPriorityJobs.enqueue(job);
        }
        //m_mutexCondition.notify_one();
    }

    void ThreadPool::Stop()
    {
        m_shouldTerminate = true;

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
                /*m_mutexCondition.wait(lock, [this] {
                    return !m_highPriorityJobs.() || !m_lowPriorityJobs.empty() || m_shouldTerminate;
                });*/
                if (m_shouldTerminate)
                    return;

                bool jobFound = m_highPriorityJobs.try_dequeue(job);
                if (!jobFound)
                {
                    jobFound = m_lowPriorityJobs.try_dequeue(job);
                    if (!jobFound)
                        continue;
                }
            }
            job();
        }
    }
}