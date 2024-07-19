// Copyright 2013, For authors. All rights reserved.
// Author: An Qin (anqin.qin@gmail.com)
//
#ifndef TOFT_CONTAINER_BLOCKING_QUEUE_H_
#define TOFT_CONTAINER_BLOCKING_QUEUE_H_

#include <queue>

#include "glog/logging.h"
#include "toft/system/threading/condition_variable.h"
#include "toft/system/threading/mutex.h"
#include "toft/system/threading/scoped_locker.h"

namespace toft {

namespace internal {
class  MutexBase;  // defined in toft/system/threading/mutex.h
}

//     A Queue that additionally supports operations that wait for the queue
// to become non-empty when retrieving an element, and wait for space to become
// available in the queue when storing an element.
//     BlockingQueue methods come in four forms, with different ways of
// handling operations that cannot be satisfied immediately, but may be
// satisfied at some point in the future: one throws an exception, the second
// returns a special value (either null or false, depending on the operation),
// the third blocks the current thread indefinitely until the operation can
// succeed, and the fourth blocks for only a given maximum time limit before
// giving up.
//
//     These methods are summarized in the following table:
//          Insert  trypush()  push(e)  push(e, timeout)
//          Remove  poll()   poll(time)
//          Check   Front()  Back()
//
//    A BlockingQueue may be capacity bounded. At any given time it may have a
// RemainingCapacity beyond which no additional elements can be put without
// blocking. A BlockingQueue without any intrinsic capacity constraints always
// reports a remaining capacity of the INT32_MAX (defined in <stdint.h>).
//
//    BlockingQueue implementations are designed to be used primarily for
// producer-consumer queues, but additionally support the Collection interface.
// So, for example, it is possible to remove an arbitrary element from a queue
// using remove(x). However, such operations are in general not performed very
// efficiently, and are intended for only occasional use, such as when a queued
// message is cancelled.
//
//    BlockingQueue implementations are thread-safe. All queuing methods
// achieve their effects atomically using internal locks or other forms of
// concurrency control.
//
//    Usage example, based on a typical producer-consumer scenario. Note that
// a BlockingQueue can safely be used with multiple producers and multiple
// consumers.
//
//    TODO(yinhaibo)  add example code

class ConditionVariable; //defined in toft/system/threading/condition_variable.h

template <typename T>
class BlockingQueue {
public:
    BlockingQueue() : m_cond(&m_mutex) {}
    ~BlockingQueue() {}

    // Inserts the specified element into this queue
    void Push(const T& e) {
        {
            ScopedLocker<toft::Mutex> locker(&m_mutex);
            m_queue.push(e);
        }
        m_cond.Broadcast();
    }

    // Retrieves and removes the head of this queue, waiting up to the specified
    // wait time if necessary for an element to become available.
    void Pop(T* e) {
        CHECK_NOTNULL(e);
        {
            ScopedLocker<toft::Mutex>  locker(&m_mutex);
            while (m_queue.size() < 1) {
                m_cond.Wait();
            }

            *e = m_queue.front();
            m_queue.pop();
        }

        return ;
    }

    bool Pop(T* e, int64_t timeout_in_ms) {
        CHECK_NOTNULL(e);
        {
            ScopedLocker<toft::Mutex>  locker(&m_mutex);
            if (m_queue.size() < 1) {
                m_cond.TimedWait(timeout_in_ms);
            }

            if (m_queue.size() >= 1) {
                *e = m_queue.front();
                m_queue.pop();
                return true;
            }
         }

        return false;
    }

    // Returns the size of elements in this queue
    int Size() {
        ScopedLocker<toft::Mutex>  locker(&m_mutex);
        return m_queue.size();
    }

    // Returns true if this queue contains more than one element and assigns the
    // frist element of queue to e;
    bool Front(T* e) {
        CHECK_NOTNULL(e);
        ScopedLocker<toft::Mutex>  locker(&m_mutex);
        if (m_queue.size() > 0) {
            *e = m_queue.front();
            return true;
        }
        return false;
    }

    // Returns true if this queue contains more than one element and assigns the
    // last element of queue to e;
    bool Back(T* e) {
        CHECK_NOTNULL(e);
        ScopedLocker<toft::Mutex>  locker(&m_mutex);
        if (m_queue.size() > 0) {
            *e = m_queue.back();
            return true;
        }
        return false;
    }

private:
    std::queue<T>  m_queue;
    toft::Mutex     m_mutex;
    toft::ConditionVariable  m_cond;
};

}  // namespace toft

#endif // TOFT_CONTAINER_BLOCKING_QUEUE_H_
