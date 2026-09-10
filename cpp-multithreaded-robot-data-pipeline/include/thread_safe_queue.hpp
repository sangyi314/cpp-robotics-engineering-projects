#pragma once

#include <condition_variable>
#include <queue>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <cstdint>

struct QueueStats
{
    std::size_t peak_size {};
    std::uint64_t push_wait_count {};
};

template <typename T>

class ThreadSafeQueue
{
    private :
        std::queue<T> m_queue ;
        std::mutex m_mutex;
        QueueStats m_stats ;

        std::condition_variable m_not_empty ;
        std::condition_variable m_not_full;

        const std::size_t m_capacity ;

        bool m_closed {false};

    public :
        explicit ThreadSafeQueue(std::size_t capacity = 64)
            :m_capacity {capacity}
            {
                if (capacity == 0)
                {
                    throw std::invalid_argument("Queue capacity must be greater than zero.");
                }
            }
        bool push (const T& value)
        {
            {
                std::unique_lock<std::mutex> lock(m_mutex);

                if(!m_closed && m_queue.size() < m_capacity)
                {
                    m_stats.push_wait_count++;
                }

                m_not_full.wait(lock,[this]{
                    return m_closed || m_queue.size() < m_capacity ;
                });

                if(m_closed)
                {
                    return false;
                }
                m_queue.push (value);

                if (m_queue.size() > m_stats.peak_size) 
                {
                    m_stats.peak_size = m_queue.size();
                }
            }
            m_not_empty.notify_one();
            return true;
        }
        std::optional<T> pop()
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_not_empty.wait(lock,[this]{return (m_closed || !m_queue.empty());});

            if (m_queue.empty())
                return std::nullopt;

            T value = m_queue.front();
            m_queue.pop();

            lock.unlock();
            return value;
        }

        void close()
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                
                m_closed = true;
            }
            m_not_empty.notify_all();
            m_not_full.notify_all();
        }

        QueueStats stats()
        {   
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_stats;
        }
};