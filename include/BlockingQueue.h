/*
 * @Author: Zhou Zijian 
 * @Date: 2023-04-21 00:49:19 
 * @Last Modified by: Zhou Zijian
 * @Last Modified time: 2023-04-21 02:28:29
 */

#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>

constexpr unsigned int TIMEOUT = 1000;
constexpr size_t DEFAULT_MAX_SIZE = 128;

template <typename T>
class BlockingQueue {
public:
    BlockingQueue() : m_maxSize(DEFAULT_MAX_SIZE)
    {}

    BlockingQueue(size_t maxSize) : m_maxSize(maxSize)
    {}

    size_t Size() const
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    bool Empty() const
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    bool Push(const T &obj, unsigned int ms = TIMEOUT)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_maxSize < m_queue.size()) {
                if (m_cvFull.wait_for(lock, std::chrono::milliseconds(ms)) == std::cv_status::timeout) {
                    return false;
                }
            }
            m_queue.push(obj);
        }
        m_cvEmpty.notify_one();
        return true;
    }

    bool Pop(T &obj, unsigned int ms = TIMEOUT)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_queue.empty()) {
                if (m_cvEmpty.wait_for(lock, std::chrono::milliseconds(ms)) == std::cv_status::timeout) {
                    return false;
                }
            }
            obj = m_queue.front();
            m_queue.pop();
        }
        m_cvFull.notify_one();
        return true;
    }

private:
    size_t m_maxSize;
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cvEmpty;
    std::condition_variable m_cvFull;
};

#endif  // BLOCKING_QUEUE_H