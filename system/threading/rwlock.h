// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_THREADING_RWLOCK_H
#define TOFT_SYSTEM_THREADING_RWLOCK_H

#include <assert.h>
#include <errno.h>
#include <pthread.h>

#include "toft/base/static_assert.h"
#include "toft/base/uncopyable.h"
#include "toft/system/check_error.h"
#include "toft/system/threading/scoped_locker.h"

namespace toft {

// Reader/Writer lock
class RwLock
{
    TOFT_DECLARE_UNCOPYABLE(RwLock);

#if defined(__GLIBC__) && __GNUC_PREREQ(3, 4)
    typedef union
    {
# if __WORDSIZE == 64
#define SIZEOF_PTHREAD_RWLOCK_T 56
        struct
        {
            int __lock;
            unsigned int __nr_readers;
            unsigned int __readers_wakeup;
            unsigned int __writer_wakeup;
            unsigned int __nr_readers_queued;
            unsigned int __nr_writers_queued;
            int __writer;
            int __pad1;
            unsigned long int __pad2;
            unsigned long int __pad3;
            /* FLAGS must stay at this position in the structure to maintain
               binary compatibility.  */
            unsigned int __flags;
        } __data;
# else
#define SIZEOF_PTHREAD_RWLOCK_T 32
        struct
        {
            int __lock;
            unsigned int __nr_readers;
            unsigned int __readers_wakeup;
            unsigned int __writer_wakeup;
            unsigned int __nr_readers_queued;
            unsigned int __nr_writers_queued;
            /* FLAGS must stay at this position in the structure to maintain
               binary compatibility.  */
            unsigned int __flags;
            int __writer;
        } __data;
# endif
        char __size[SIZEOF_PTHREAD_RWLOCK_T];
        long int __align;
    } PthreadRwlock;
#endif

public:
    typedef ScopedReaderLocker<RwLock> ReaderLocker;
    typedef ScopedWriterLocker<RwLock> WriterLocker;
    typedef ScopedTryReaderLocker<RwLock> TryReaderLocker;
    typedef ScopedTryWriterLocker<RwLock> TryWriterLocker;

    enum Kind {
        kKindPreferReader = PTHREAD_RWLOCK_PREFER_READER_NP,

        // Setting the value read-write lock kind to PTHREAD_RWLOCK_PREFER_WRITER_NP,
        // results in the same behavior as setting the value to PTHREAD_RWLOCK_PREFER_READER_NP.
        // As long as a reader thread holds the lock the thread holding a write lock will be
        // starved. Setting the kind value to PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP,
        // allows the writer to run. However, the writer may not be recursive as is implied by the
        // name.
        kKindPreferWriter = PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP,
        kKindDefault = PTHREAD_RWLOCK_DEFAULT_NP,
    };

public:
    RwLock();
    explicit RwLock(Kind kind);
    ~RwLock();

    void ReaderLock()
    {
        CheckValid();
        TOFT_CHECK_PTHREAD_ERROR(pthread_rwlock_rdlock(&m_lock));
    }

    bool TryReaderLock()
    {
        CheckValid();
        return TOFT_CHECK_PTHREAD_TRYLOCK_ERROR(pthread_rwlock_tryrdlock(&m_lock));
    }

    void WriterLock()
    {
        CheckValid();
        TOFT_CHECK_PTHREAD_ERROR(pthread_rwlock_wrlock(&m_lock));
    }

    bool TryWriterLock()
    {
        CheckValid();
        return TOFT_CHECK_PTHREAD_TRYLOCK_ERROR(pthread_rwlock_trywrlock(&m_lock));
    }

    void Unlock()
    {
        CheckValid();
        TOFT_CHECK_PTHREAD_ERROR(pthread_rwlock_unlock(&m_lock));
    }

    // Names for scoped lockers
    void ReaderUnlock()
    {
        Unlock();
    }

    void WriterUnlock()
    {
        Unlock();
    }

public: // Only for test and debug, should not be used in normal code logical
    void AssertReaderLocked()
    {
        // Accessing pthread private data: nonportable but no other way
#if defined(__GLIBC__)
#if __GNUC_PREREQ(7, 0)
        assert(m_lock.__data.__readers != 0);
#elif __GNUC_PREREQ(4, 1)
        assert(m_lock.__data.__nr_readers != 0);
#elif __GNUC_PREREQ(3, 4)
        const PthreadRwlock* rwlock =
            reinterpret_cast<const PthreadRwlock*>(&m_lock);
        (void)rwlock; // disable complier to unused check
        assert(rwlock->__data.__nr_readers != 0);
#else
        TOFT_STATIC_ASSERT(true, "the glibc version is too old");
#endif
#else
        TOFT_STATIC_ASSERT(true, "unsupported library");
#endif
    }

    void AssertWriterLocked() const
    {
#if defined(__GLIBC__)
#if __GNUC_PREREQ(7, 0)
        assert(m_lock.__data.__writers != 0);
#elif __GNUC_PREREQ(4, 1)
        assert(m_lock.__data.__writer != 0);
#elif __GNUC_PREREQ(3, 4)
        const PthreadRwlock* rwlock =
            reinterpret_cast<const PthreadRwlock*>(&m_lock);
        (void)rwlock; // disable complier to unused check
        assert(rwlock->__data.__writer != 0);
#else
        TOFT_STATIC_ASSERT(true, "the glibc version is too old");
#endif
#else
        TOFT_STATIC_ASSERT(true, "unsupported library");
#endif
    }

    void AssertLocked()
    {
#if defined(__GLIBC__)
#if __GNUC_PREREQ(7, 0)
        assert(m_lock.__data.__readers != 0 || m_lock.__data.__writers != 0);
#elif __GNUC_PREREQ(4, 1)
        assert(m_lock.__data.__nr_readers != 0 || m_lock.__data.__writer != 0);
#elif __GNUC_PREREQ(3, 4)
        const PthreadRwlock* rwlock =
            reinterpret_cast<const PthreadRwlock*>(&m_lock);
        assert(rwlock->__data.__nr_readers != 0
               || rwlock->__data.__writer != 0);
        (void)rwlock; // disable complier to unused check
#else
        TOFT_STATIC_ASSERT(true, "the glibc version is too old");
#endif
#else
        TOFT_STATIC_ASSERT(true, "unsupported library");
#endif
    }

private:
    void CheckValid() const
    {
#if defined(__GLIBC__)
#if __GNUC_PREREQ(4, 1)
        // If your program crashed here at runtime, maybe the rwlock object
        // has been destructed.
        if (m_lock.__data.__flags == 0xFFFFFFFFU) {
            TOFT_CHECK_ERRNO_ERROR(EINVAL);
        }
#elif __GNUC_PREREQ(3, 4)
        const PthreadRwlock* rwlock =
            reinterpret_cast<const PthreadRwlock*>(&m_lock);
        if (rwlock->__data.__flags == 0xFFFFFFFFU) {
            TOFT_CHECK_ERRNO_ERROR(EINVAL);
        }
#else
        TOFT_STATIC_ASSERT(true, "the glibc version is too old");
#endif
#else
        TOFT_STATIC_ASSERT(true, "unsupported library");
#endif
    }

private:
    pthread_rwlock_t m_lock;
};

} // namespace toft

#endif // TOFT_SYSTEM_THREADING_RWLOCK_H
