// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_THREADING_MUTEX_H
#define TOFT_SYSTEM_THREADING_MUTEX_H

#include <assert.h>
#include <errno.h>

#include <pthread.h>

#include <string.h>
#include <stdexcept>
#include <string>

#include "toft/base/static_assert.h"
#include "toft/base/uncopyable.h"
#include "toft/system/check_error.h"
#include "toft/system/threading/scoped_locker.h"

namespace toft {

class ConditionVariable;

namespace internal {

class MutexBase
{
    TOFT_DECLARE_UNCOPYABLE(MutexBase);

#if defined(__GLIBC__) && __GNUC_PREREQ(3, 4)
    typedef union
    {
        struct
        {
            int __lock;
            unsigned int __count;
            int __owner;
#if __WORDSIZE == 64
#define SIZEOF_PTHREAD_MUTEX_T 40
            unsigned int __nusers;
#endif
            /* KIND must stay at this position in the structure to maintain
               binary compatibility.  */
            int __kind;
#if __WORDSIZE != 64
#define SIZEOF_PTHREAD_MUTEX_T 24
            unsigned int __nusers;
#endif
            int __spins;
        } __data;
        char __size[SIZEOF_PTHREAD_MUTEX_T];
        long int __align;
    } PthreadMutex;
#endif

protected:
    // Mutex type converter
    explicit MutexBase(int type);
    ~MutexBase();
public:
    void Lock()
    {
        TOFT_CHECK_PTHREAD_ERROR(pthread_mutex_lock(&m_mutex));
        AssertLocked();
    }

    bool TryLock()
    {
        return TOFT_CHECK_PTHREAD_TRYLOCK_ERROR(
            pthread_mutex_trylock(&m_mutex));
    }

    // for test and debug only
    void AssertLocked() const
    {
        // by inspect internal data
#if defined(__GLIBC__)
#if __GNUC_PREREQ(4, 1)
        assert(m_mutex.__data.__lock > 0);
#elif __GNUC_PREREQ(3, 4)
        const PthreadMutex* mutex =
            reinterpret_cast<const PthreadMutex*>(&m_mutex);
        (void)mutex;
        assert(mutex->__data.__lock > 0);
        (void)mutex; // disable compiler to do unused check;
#else
        TOFT_STATIC_ASSERT(true, "the glibc version is too old");
#endif
#else
        TOFT_STATIC_ASSERT(true, "unsupported library");
#endif
    }

    void Unlock()
    {
        AssertLocked();
        TOFT_CHECK_PTHREAD_ERROR(pthread_mutex_unlock(&m_mutex));
        // NOTE: can't check unlocked here, maybe already locked by other thread
    }
private:
    friend class ::toft::ConditionVariable;
    pthread_mutex_t m_mutex;
};

} // namespace internal

/// if same thread try to acquire the lock twice, deadlock would occur.
class Mutex : public internal::MutexBase
{
public:
    typedef ScopedLocker<Mutex> Locker;
    Mutex() : internal::MutexBase(PTHREAD_MUTEX_DEFAULT)
    {
    }
};

// RecursiveMutex can be acquired by same thread multiple times, but slower than
// plain Mutex
class RecursiveMutex : public internal::MutexBase
{
public:
    typedef ScopedLocker<RecursiveMutex> Locker;
    RecursiveMutex() : internal::MutexBase(PTHREAD_MUTEX_RECURSIVE)
    {
    }
};

/// try to spin some time if can't acquire lock, if still can't acquire, wait.
class AdaptiveMutex : public internal::MutexBase
{
public:
    typedef ScopedLocker<AdaptiveMutex> Locker;
    AdaptiveMutex() : internal::MutexBase(PTHREAD_MUTEX_ADAPTIVE_NP)
    {
    }
};

typedef ScopedLocker<internal::MutexBase> MutexLocker;

// Check ing missing variable name, eg MutexLocker(m_lock);
#define MutexLocker(x) STATIC_ASSERT(false, "Mising variable name of MutexLocker")

// Null mutex for template mutex param placeholder
// NOTE: don't make this class uncopyable
class NullMutex
{
public:
    typedef ScopedLocker<NullMutex> Locker;
public:
    NullMutex() : m_locked(false)
    {
    }

    void Lock()
    {
        m_locked = true;
    }

    bool TryLock()
    {
        m_locked = true;
        return true;
    }

    // by inspect internal data
    bool IsLocked() const
    {
        return m_locked;
    }

    void Unlock()
    {
        m_locked = false;
    }
private:
    bool m_locked;
};

} // namespace toft

#endif // TOFT_SYSTEM_THREADING_MUTEX_H

