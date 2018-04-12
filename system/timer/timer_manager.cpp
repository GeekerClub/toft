// Copyright 2012, For toft authors.
// Author: An Qin (anqin.qin@gmail.com)

#include "toft/system/timer/timer_manager.h"

#include "toft/system/atomic/type.h"
#include "toft/system/time/timestamp.h"

namespace toft {


TimerManager::TimerManager() : m_mutex(), m_cond(&m_mutex), m_running_timer(0) {
    StartThread();
}

uint64_t TimerManager::AddTimer(int64_t interval, bool is_period,
                                CallbackClosure* closure) {
    assert(interval >= 0);
    assert(!IsStoped());

    MutexLocker locker(&m_mutex);
    uint64_t id = NewTimerId();
    TimerEntry& timer = m_timers[id];
    timer.interval = interval;
    timer.is_period = is_period;
    timer.closure = closure;
    timer.is_enabled = true;

    SetNextTimeout(id, interval, 0);

    return id;
}

uint64_t TimerManager::AddOneshotTimer(int64_t interval,
                                       CallbackClosure* closure) {
    return AddTimer(interval, false, closure);
}

uint64_t TimerManager::AddPeriodTimer(int64_t interval,
                                      CallbackClosure* closure) {
    return AddTimer(interval, true, closure);
}

bool TimerManager::ModifyTimer(uint64_t id, int64_t interval,
                               CallbackClosure* closure) {
    assert(!IsStoped());

    MutexLocker locker(&m_mutex);
    TimerEntry* entry = FindEntry(id);
    if (entry) {
        entry->interval = interval;
        if (entry->closure != closure) {
            delete entry->closure; // release the old closure
            entry->closure = closure;
        }
        entry->revision++;
        if (entry->is_enabled) {
            SetNextTimeout(id, interval, entry->revision);
        }
        return true;
    }
    return false;
}

bool TimerManager::ModifyTimer(uint64_t id, int64_t interval) {
    assert(!IsStoped());

    MutexLocker locker(&m_mutex);
    TimerEntry* entry = FindEntry(id);
    if (entry) {
        entry->interval = interval;
        entry->revision++;
        if (entry->is_enabled) {
            SetNextTimeout(id, interval, entry->revision);
        }
        return true;
    }
    return false;
}

bool TimerManager::RemoveTimer(uint64_t id) {
    assert(!IsStoped());

    // 0 is an invalid id
    if (id == 0) {
        return false;
    }

    if (ThisThread::GetId() == GetId()) {
        // in the same thread, can call AsyncRemoveTimer
        return AsyncRemoveTimer(id);
    }

    while (true) {
        {
            MutexLocker locker(&m_mutex);
            if (id != m_running_timer) {
                return AsyncRemoveTimerNoLock(id);
            }
        }
        ThisThread::Sleep(1);
    }
}

bool TimerManager::AsyncRemoveTimer(uint64_t id) {
    assert(!IsStoped());

    MutexLocker locker(&m_mutex);

    return AsyncRemoveTimerNoLock(id);
}

bool TimerManager::AsyncRemoveTimerNoLock(uint64_t id) {
    TimerMap::iterator it = m_timers.find(id);
    if (it != m_timers.end()) {
        delete it->second.closure;
        m_timers.erase(it);
        return true;
    }
    return false;
}

bool TimerManager::DisableTimer(uint64_t id) {
    assert(!IsStoped());

    MutexLocker locker(&m_mutex);
    TimerEntry* entry = FindEntry(id);
    if (entry) {
        entry->is_enabled = false;
        entry->revision++;
        return true;
    }
    return false;
}

bool TimerManager::EnableTimer(uint64_t id) {
    assert(!IsStoped());

    MutexLocker locker(&m_mutex);
    TimerEntry* entry = FindEntry(id);
    if (entry) {
        entry->is_enabled = true;
        SetNextTimeout(id, entry->interval, entry->revision);
        return true;
    }
    return false;
}

// -------------------------------------------------------
// the functions below are declared private
// mutex is not needed because they are aready under lock.
// -------------------------------------------------------
TimerManager::TimerEntry* TimerManager::FindEntry(uint64_t id) {

    TimerMap::iterator it = m_timers.find(id);
    if (it != m_timers.end()) {
        return &it->second;
    }
    return NULL;
}

bool TimerManager::GetLatestTime(int64_t* time) const {

    if (m_timeouts.empty()) {
        return false;
    }

    *time = m_timeouts.top().time;
    return true;
}

bool TimerManager::GetLatestTimeout(int* timeout) const {
    int64_t latest_time = 0;
    if (!GetLatestTime(&latest_time)) {
        return false;
    }
    int64_t now = GetTimestampInMs();
    if (now < latest_time) {
        *timeout = latest_time - now;
    } else {
        *timeout = 0;
    }
    return true;
}

uint64_t TimerManager::NewTimerId() {
    static Atomic<uint64_t> timer_id(0);
    return ++timer_id;
}

void TimerManager::PushNextTimeout(uint64_t id, int64_t interval,
                                   uint8_t revision) {
    Timeout timeout = {GetTimestampInMs() + interval, id, revision};
    m_timeouts.push(timeout);
}

void TimerManager::SetNextTimeout(uint64_t id, int64_t interval,
                                  uint8_t revision) {

    int64_t prev_top_time;

    if (ThisThread::GetId() == GetId()) {
        PushNextTimeout(id, interval, revision);
    } else if (GetLatestTime(&prev_top_time)) {
        PushNextTimeout(id, interval, revision);
        int64_t new_top_time = m_timeouts.top().time;
        // reschedule timer only if the new time is the most early
        if (prev_top_time > new_top_time)
            m_cond.Signal();
    } else {
        // timer manager is empty, worker thread is waiting infinitely
        PushNextTimeout(id, interval, revision);
        m_cond.Signal();
    }
}

// -------------------------------------------------------

void TimerManager::Entry() {
    while (true) {
        {
            MutexLocker locker(&m_mutex);
            if (BaseThread::IsStopRequested()) {
                break;
            }

            int timeout;
            if (GetLatestTimeout(&timeout)) {
                m_cond.TimedWait(timeout);
            } else {
                m_cond.Wait();
            }
        }
        Dispatch();
    }
}

void TimerManager::StartThread() {
    if (!BaseThread::TryStart()) {
        std::string msg = strerror(errno);
        throw std::runtime_error("Can't start timer thread: " + msg);
    }
}

bool TimerManager::IsStoped() const {
    // NOTE: BaseThread::Running is not reliable, because thread maybe already
    // created but not runing
    return GetId() <= 0;
}

void TimerManager::Stop() {
    if (!IsStoped()) {
        {
            MutexLocker locker(&m_mutex);
            BaseThread::SendStopRequest();
            m_cond.Signal();
        }
        BaseThread::Join();
        Clear();
    }
}

bool TimerManager::DequeueTimeoutEntry(uint64_t* id, TimerEntry* entry) {
    while (true) {
        MutexLocker locker(&m_mutex);

        int64_t now = GetTimestampInMs();
        if (m_timeouts.empty() || m_timeouts.top().time > now) {
            return false;
        }

        Timeout timeout = m_timeouts.top();
        m_timeouts.pop();

        TimerMap::iterator it = m_timers.find(timeout.timer_id);
        if (it == m_timers.end()) {
            continue;
        }

        // ignore outdated timeouts
        if (!it->second.is_enabled || timeout.revision != it->second.revision) {
            continue;
        }

        *id = timeout.timer_id;
        *entry = it->second;

        if (entry->is_period) {
            // move the closure into the entry temporarily during running,
            // make sure the closure will not be deleted in calling ModifyTimer
            // and RemoveTimer
            it->second.closure = NULL;
        } else {
            m_timers.erase(it);
        }

        m_running_timer = *id;
        break;
    }

    return true;
}

void TimerManager::Dispatch() {
    uint64_t id;

    // Make a copy. It's necessary because the entry in the map maybe changed
    // by it's closure.
    TimerEntry entry;

    while (DequeueTimeoutEntry(&id, &entry)) {
        // Run the closure in unlocked state
        if (entry.closure != NULL) {
            entry.closure->Run(id);
        }

        MutexLocker locker(&m_mutex);
        m_running_timer = 0;
        if (entry.is_period) {
            TimerMap::iterator it = m_timers.find(id);
            if (it != m_timers.end()) {
                // restore the closure if necessory
                if (entry.closure != NULL) {
                    if (it->second.closure == NULL) {
                        it->second.closure = entry.closure;
                    } else {
                        // the entry in the map has been assign a new one,
                        // release the old closure;
                        delete entry.closure;
                    }
                }
                if (it->second.is_enabled) {
                    SetNextTimeout(it->first, it->second.interval,
                                   it->second.revision);
                }
            } else {
                // the timer has been removed
                delete entry.closure;
            }
        }
    }
}

void TimerManager::Clear() {
    MutexLocker locker(&m_mutex);
    while (!m_timeouts.empty()) {
        m_timeouts.pop();
    }

    TimerMap::iterator iter = m_timers.begin();
    for (; iter != m_timers.end(); ++iter) {
        delete iter->second.closure;
    }
    m_timers.clear();
}

TimerManager::~TimerManager() {
    Stop();
}

}  // namespace toft
