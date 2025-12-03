#include <wv/threading/ThreadPool.h>

#include <wv/Logger.h>

namespace WillowVox
{
    ThreadPool::ThreadPool()
        : m_shouldTerminate(false) {}

    ThreadPool::~ThreadPool()
    {
        m_shouldTerminate = true;

        // Make sure all threads stop
        for (int i = 0; i < m_threads.size() * 2; i++)
            m_signal.enqueue(true);

        // Join all threads
        for (std::thread& activeThread : m_threads)
            activeThread.join();

        m_threads.clear();
    }

    void ThreadPool::Start(int numThreads)
    {
        for (int i = 0; i < numThreads; i++)
            m_threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
    }

    void ThreadPool::Enqueue(const std::function<void()>& job, Priority priority)
    {
        // Enqueue the job
        m_queues[static_cast<int>(priority)].enqueue(job);
        // Wake up a worker thread to run the job
        m_signal.enqueue(true);
    }

    void ThreadPool::ThreadLoop()
    {
        bool token;
        while (true)
        {
            // Wait for job to be enqueued
            m_signal.wait_dequeue(token);
            Logger::Log("Worker thread %d checking for jobs", std::this_thread::get_id());

            // Stop early if necessary
            if (m_shouldTerminate)
                return;

            // Get job to run
            std::function<void()> job;
            bool found = false;

            for (int i = 0; i < static_cast<int>(Priority::Count); i++)
            {
                if (m_queues[static_cast<int>(Priority::High)].try_dequeue(job))
                {
                    found = true;
                    break;
                }
            }

            // Run job if found
            if (found && job)
                job();
        }
    }
}