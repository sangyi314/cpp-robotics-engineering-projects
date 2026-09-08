#pragma once

#include <condition_variable>
#include <queue>
#include <mutex>

template <typename T>

class ThreadSafeQueue
{
    private :
        std::queue<T> m_queue ;
        std::mutex m_mutex;
        std::condition_variable m_condition;
    public :
        void push (const T& value)
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_queue.push (value);
            }
            m_condition.notify_one();
        }
        T pop()
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_condition.wait(lock,[this]{return !m_queue.empty();});

            T value = m_queue.front();
            m_queue.pop();
            return value;
        }
};