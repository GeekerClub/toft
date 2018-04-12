// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_SYSTEM_CONCURRENCY_BLOCKING_QUEUE_H
#define COMMON_SYSTEM_CONCURRENCY_BLOCKING_QUEUE_H

/// @file
/// @author phongchen <phongchen@tencent.com>

#include <assert.h>
#include <cstddef>
#include <deque>
#include "toft/base/uncopyable.h"
#include "toft/system/threading/condition_variable.h"
#include "toft/system/threading/mutex.h"

namespace toft {

/// @brief producer/consumer queue
template <typename T>
class BlockingQueue : private Uncopyable
{
public:
    typedef T ValueType;

    BlockingQueue(size_t max_elements = static_cast<size_t>(-1))
        : m_max_elements(max_elements)
    {
        assert(m_max_elements > 0);
    }

    /// @brief push element in to front of queue
    /// @param value to be pushed
    /// @note if queue is full, block and wait for non-full
    void PushFront(const T& value)
    {
        {
            MutexLocker locker(m_mutex);
            while (UnlockedIsFull())
            {
                m_cond_not_full.Wait(m_mutex);
            }
            m_queue.push_front(value);
        }
        m_cond_not_empty.Signal();
    }

    /// @brief push element in to back of queue
    /// @param value to be pushed
    /// @note if queue is full, block and wait for non-full
    void PushBack(const T& value)
    {
        {
            MutexLocker locker(m_mutex);
            while (UnlockedIsFull())
            {
                m_cond_not_full.Wait(m_mutex);
            }
            m_queue.push_back(value);
        }
        m_cond_not_empty.Signal();
    }

    /// @brief popup from front of queue.
    /// @param value to hold the result
    /// @note if queue is empty, block and wait for non-empty
    void PopFront(T* value)
    {
        {
            MutexLocker locker(m_mutex);
            while (m_queue.empty())
            {
                m_cond_not_empty.Wait(m_mutex);
            }
            *value = m_queue.front();
            m_queue.pop_front();
        }
        m_cond_not_full.Signal();
    }

    /// @brief popup from back of queue
    /// @param value to hold the result
    /// @note if queue is empty, block and wait for non-empty
    void PopBack(T* value)
    {
        {
            MutexLocker locker(m_mutex);
            while (m_queue.empty())
            {
                m_cond_not_empty.Wait(m_mutex);
            }
            *value = m_queue.back();
            m_queue.pop_back();
        }
        m_cond_not_full.Signal();
    }

    /// @brief push front with time out
    /// @param value to be pushed
    /// @param timeout_in_ms timeout, in milliseconds
    /// @return whether pushed
    bool TimedPushFront(const T& value, int timeout_in_ms)
    {
        bool success = false;
        {
            MutexLocker locker(m_mutex);

            if (UnlockedIsFull())
                m_cond_not_full.Wait(m_mutex, timeout_in_ms);

            if (!UnlockedIsFull())
            {
                m_queue.push_front(value);
                success = true;
            }
        }

        if (success)
            m_cond_not_empty.Signal();

        return success;
    }

    /// @brief push back with time out
    /// @param value to be pushed
    /// @param timeout_in_ms timeout, in milliseconds
    /// @return whether pushed
    bool TimedPushBack(const T& value, int timeout_in_ms)
    {
        bool success = false;
        {
            MutexLocker locker(m_mutex);
            if (UnlockedIsFull())
                m_cond_not_full.Wait(m_mutex, timeout_in_ms);

            if (!UnlockedIsFull())
            {
                m_queue.push_back(value);
                success = true;
            }
        }
        if (success)
            m_cond_not_empty.Signal();
        return success;
    }

    /// @brief pop front with time out
    /// @param value to hold the result
    /// @param timeout_in_ms timeout, in milliseconds
    /// @return whether poped
    bool TimedPopFront(T* value, int timeout_in_ms)
    {
        bool success = false;
        {
            MutexLocker locker(m_mutex);

            if (m_queue.empty())
                m_cond_not_empty.Wait(m_mutex, timeout_in_ms);

            if (!m_queue.empty())
            {
                *value = m_queue.front();
                m_queue.pop_front();
                success = true;
            }
        }

        if (success)
            m_cond_not_full.Signal();

        return success;
    }

    /// @brief pop back with time out
    /// @param value to hold the result
    /// @param timeout_in_ms timeout, in milliseconds
    /// @return whether poped
    bool TimedPopBack(T* value, int timeout_in_ms)
    {
        bool success = false;
        {
            MutexLocker locker(m_mutex);

            if (m_queue.empty())
                m_cond_not_empty.Wait(m_mutex, timeout_in_ms);

            if (!m_queue.empty())
            {
                *value = m_queue.back();
                m_queue.pop_back();
                success = true;
            }
        }

        if (success)
            m_cond_not_full.Signal();

        return success;
    }

    /// @brief pop all from the queue and guarantee the output queue contains at
    /// least one element.
    /// @param values to hold the results, it would be cleared firstly by the
    /// function.
    /// @note if queue is empty, block and wait for non-empty.
    void PopAll(std::deque<T>* values)
    {
        values->clear();

        {
            MutexLocker locker(m_mutex);
            while (m_queue.empty())
                m_cond_not_empty.Wait(m_mutex);
            values->swap(m_queue);
        }

        m_cond_not_full.Broadcast();
    }

    /// @brief pop all from the queue with time out.
    /// @param values to hold the results, it would be cleared firstly by the
    /// function.
    /// @param timeout_in_ms timeout, in milliseconds.
    /// @return whether poped.
    bool TimedPopAll(std::deque<T>* values, int timeout_in_ms)
    {
        bool success = false;
        values->clear();

        {
            MutexLocker locker(m_mutex);
            if (m_queue.empty())
                m_cond_not_empty.Wait(m_mutex, timeout_in_ms);

            if (!m_queue.empty())
            {
                values->swap(m_queue);
                m_cond_not_full.Broadcast();
                success = true;
            }
        }

        if (success)
            m_cond_not_full.Signal();

        return success;
    }

    /// @brief number of elements in the queue.
    /// @return number of elements in the queue.
    size_t Size() const
    {
        MutexLocker locker(m_mutex);
        return m_queue.size();
    }

    /// @brief whether the queue is empty
    /// @return whether empty
    bool IsEmpty() const
    {
        MutexLocker locker(m_mutex);
        return m_queue.empty();
    }

    /// @brief whether the queue is full
    /// @return whether full
    bool IsFull() const
    {
        MutexLocker locker(m_mutex);
        return UnlockedIsFull();
    }

    /// @brief clear the queue
    void Clear()
    {
        MutexLocker locker(m_mutex);
        m_queue.clear();
        m_cond_not_full.Broadcast();
    }
private:
    bool UnlockedIsFull() const
    {
        return m_queue.size() >= m_max_elements;
    }
private:
    size_t m_max_elements;
    std::deque<T> m_queue;

    mutable SimpleMutex m_mutex;
    ConditionVariable m_cond_not_empty;
    ConditionVariable m_cond_not_full;
};
} // namespace toft;
#endif // COMMON_SYSTEM_CONCURRENCY_BLOCKING_QUEUE_H

