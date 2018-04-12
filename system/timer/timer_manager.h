// Copyright 2012, For toft authors.
// Author: An Qin (anqin.qin@gmail.com)

#ifndef TOFT_SYSTEM_TIMER_TIMER_MANAGER_H_
#define TOFT_SYSTEM_TIMER_TIMER_MANAGER_H_
#pragma once


#include <functional>
#include <map>
#include <queue>
#include <string>
#include <vector>

#include "toft/base/closure.h"
#include "toft/system/threading/base_thread.h"
#include "toft/system/threading/condition_variable.h"
#include "toft/system/threading/mutex.h"


namespace toft {

// timer manager class. Register and delete timers.
class TimerManager : private BaseThread {
public:
    TimerManager();
    virtual ~TimerManager();

    // timer callback closure, input param: Timer ID.
    typedef Closure<void(uint64_t timer_id)> CallbackClosure;


    // add a one-thot timer,
    // interval, in milliseconds; closure should be self delete
    // return a unique timer id
    uint64_t AddOneshotTimer(int64_t interval, CallbackClosure* closure);

    // add a periodly timer
    // interval, in milliseconds; closure should be permanent
    // return a unique timer id
    uint64_t AddPeriodTimer(int64_t interval, CallbackClosure* closure);

    // modify a timer using new interval and closure
    // interval, in milliseconds; new closure should be different with old
    // clousre, and old closure will be deleted.
    bool ModifyTimer(uint64_t id, int64_t interval, CallbackClosure* closure);

    // modify a timer using new time interval
    bool ModifyTimer(uint64_t id, int64_t interval);

    // disable a timer
    bool DisableTimer(uint64_t id);

    // enable a timer
    bool EnableTimer(uint64_t id);

    // remove a timer synchronous
    bool RemoveTimer(uint64_t id);

    // remove a timer asynchronous
    bool AsyncRemoveTimer(uint64_t id);

    // stop the timer dispatch thread and clear all timers
    void Stop();

    // Whether Stop has been called
    bool IsStoped() const;

    // remove all timers in current timer manager
    void Clear();

private:
    struct TimerEntry {
        CallbackClosure* closure;   // closure
        int64_t interval;           // timer interval
        bool is_enabled;            // whether timer is enabled
        bool is_period;             // whether timer run periodly
        uint8_t revision;           // modified revision

        TimerEntry() : interval(0), is_enabled(false),
                is_period(false), revision(0) {}
    };

    struct Timeout {
        int64_t time;       // time when the timer triggers
        uint64_t timer_id;  // timer id
        uint8_t revision;   // timer revision, the old will be discarded

        bool operator > (const Timeout& rhs) const {
            return time > rhs.time;
        }
    };

    typedef std::map<uint64_t, TimerEntry> TimerMap;

private:
    // timer thread start
    void StartThread();

    // timer thread entrance.
    virtual void Entry();

    TimerEntry* FindEntry(uint64_t id);

    bool DequeueTimeoutEntry(uint64_t* id, TimerEntry* entry);

    // process timeouts
    void Dispatch();

    // auto generate a new timer id
    uint64_t NewTimerId();

    bool GetLatestTime(int64_t* time) const;
    bool GetLatestTimeout(int* timeout) const;

    // add next timeout to the timeout list.
    void SetNextTimeout(uint64_t id, int64_t interval, uint8_t revision);

    void PushNextTimeout(uint64_t id, int64_t interval, uint8_t revision);

    // internal use. add a timer.
    uint64_t AddTimer(int64_t interval, bool is_period,
                      CallbackClosure* closure);

    bool AsyncRemoveTimerNoLock(uint64_t id);

private:
    mutable Mutex m_mutex;
    ConditionVariable m_cond;
    // times container
    TimerMap m_timers;
    // timeouts queue, latest timeout first.
    std::priority_queue<Timeout, std::vector<Timeout>,
            std::greater<Timeout> > m_timeouts;
    volatile uint64_t m_running_timer;
};

}  // namespace toft

#endif  // TOFT_SYSTEM_TIMER_TIMER_MANAGER_H_
