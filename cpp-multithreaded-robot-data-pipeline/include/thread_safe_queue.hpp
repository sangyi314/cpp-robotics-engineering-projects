#pragma once

#include <condition_variable>
#include <queue>
#include <mutex>
#include <optional>

template <typename T>

class ThreadSafeQueue
{
    private :
        std::queue<T> m_queue ;
        std::mutex m_mutex;
        std::condition_variable m_condition;
        bool m_closed {false};
    public :
        bool push (const T& value)
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);

                if (m_closed)
                    return false;
                
                m_queue.push (value);
            }
            m_condition.notify_one();
            return true;
        }
        std::optional<T> pop()
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_condition.wait(lock,[this]{return (m_closed || !m_queue.empty());});

            if (m_queue.empty())
                return std::nullopt;

            T value = m_queue.front();
            m_queue.pop();
            return value;
        }

        void close()
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                
                m_closed = true;
            }
            m_condition.notify_all();
        }
};