// Copyright (C) 2016, For authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:


#include "toft/container/counter.h"

#include <stdlib.h>

#include "toft/system/threading/thread_pool.h"
#include "toft/system/timer/timer_manager.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(toft_counter_output_interval_in_s, 60,
             "the interval to dump counter statitics");
DEFINE_bool(toft_counter_enabled, false,
            "enable the counter statitics");

void CounterManager::Init(TimerManager* timer_manager, ThreadPool* thread_pool,
                          CounterHandler* counter_handler) {
    std::vector<CounterHandler*> counter_handlers;
    counter_handlers.push_back(counter_handler);
    Init(timer_manager, thread_pool, counter_handlers);
}

void CounterManager::Init(TimerManager* timer_manager,
                          ThreadPool* thread_pool,
                          std::vector<CounterHandler*>& counter_handlers) {
    std::vector<CounterCallback*> counter_callbacks;
    Init(timer_manager, thread_pool, counter_handlers, counter_callbacks);
}

void CounterManager::Init(TimerManager* timer_manager, ThreadPool* thread_pool,
                          CounterCallback* counter_callback) {
    std::vector<CounterCallback*> counter_callbacks;
    counter_callbacks.push_back(counter_callback);
    Init(timer_manager, thread_pool, counter_callbacks);
}

void CounterManager::Init(TimerManager* timer_manager,
                          ThreadPool* thread_pool,
                          std::vector<CounterCallback*>& counter_callbacks) {
    std::vector<CounterHandler*> counter_handlers;
    Init(timer_manager, thread_pool, counter_handlers, counter_callbacks);
}

void CounterManager::Init(TimerManager* timer_manager,
                          ThreadPool* thread_pool,
                          std::vector<CounterHandler*>& counter_handlers,
                          std::vector<CounterCallback*>& counter_callbacks) {
    CHECK_NOTNULL(timer_manager);
    CHECK_NOTNULL(thread_pool);
    LOG(INFO) << "CounterManager::Init";

    MutexLocker locker(&m_mutex);
    m_timer_manager = timer_manager;
    m_thread_pool = thread_pool;
    m_counter_handlers.assign(counter_handlers.begin(), counter_handlers.end());
    m_counter_callbacks.assign(counter_callbacks.begin(), counter_callbacks.end());

    if (FLAGS_toft_counter_enabled) {
        Closure<void (uint64_t)>* closure =
            NewClosure(this, &CounterManager::ReportCounterWrapper);

        m_timer_id = m_timer_manager->AddOneshotTimer(
            FLAGS_toft_counter_output_interval_in_s * 1000,
            closure);
        if (m_timer_id == 0) {
            delete closure;
        }
        LOG(INFO) << "add timer id = " << m_timer_id;
    }
}

void CounterManager::Terminate() {
    LOG(INFO) << "CounterManager::Terminate";

    if (!FLAGS_toft_counter_enabled) {
        CleanHandlers();
        return;
    }

    bool removed = false;
    {
        MutexLocker locker(&m_mutex);

        CHECK_NOTNULL(m_timer_manager);
        CHECK_NOTNULL(m_thread_pool);

        removed = m_timer_manager->RemoveTimer(m_timer_id);
        if (!removed) {
            LOG(WARNING) << "Remove Timer failed.";
        }
    }

    if (removed) {
        // Report counter for the last time.
        ReportCounter();
        CHECK(m_timer_manager->RemoveTimer(m_timer_id));
    }
}

void CounterManager::CleanHandlers() {
    for (uint32_t i = 0; i < m_counter_callbacks.size(); ++i) {
        delete m_counter_callbacks[i];
    }
}

void CounterManager::ReportCounter() {
    MutexLocker locker(&m_mutex);

    std::list<CounterHandler::RateRecord> rate_records;
    std::list<CounterHandler::ValueRecord> value_records;
    std::list<CounterHandler::MapRecord> map_records;
    std::list<CounterHandler::SumRecord> sum_records;
    for (std::list<CounterPointer>::iterator iter = m_counters.begin();
        iter != m_counters.end(); ++iter) {
        if (iter->m_type == Counter_Type_Rate) {

            // Report the current value, and set current value to 0.
            uint64_t value = 0;
            iter->m_rate_counter->GetAndReset(&value);
            rate_records.push_back(CounterHandler::RateRecord(
                iter->m_rate_counter->m_name, value));
        } else if (iter->m_type == Counter_Type_Value) {
            int64_t avg = 0;
            int64_t max = 0;
            int64_t min = 0;
            iter->m_value_counter->GetAndReset(&avg, &max, &min);
            value_records.push_back(CounterHandler::ValueRecord(
                iter->m_value_counter->GetName(),
                min,
                max,
                avg));
        } else if (iter->m_type == Counter_Type_Map) {
            std::map<std::string, int64_t> map;
            iter->m_map_counter->GetAndReset(&map);
            for (std::map<std::string, int64_t>::iterator it = map.begin();
                 it != map.end(); ++it) {
                map_records.push_back(CounterHandler::MapRecord(
                        iter->m_map_counter->GetName(),
                        it->first,
                        it->second));
            }
        } else {
            sum_records.push_back(CounterHandler::SumRecord(
                iter->m_sum_counter->m_name, iter->m_sum_counter->GetValue()));
        }
    }

    for (size_t i = 0; i < m_counter_handlers.size(); ++i) {
        m_counter_handlers[i]->HandleData(rate_records, value_records, map_records, sum_records);
    }

    for (size_t i = 0; i < m_counter_callbacks.size(); ++i) {
        m_counter_callbacks[i]->Run(rate_records, value_records, map_records, sum_records);
    }

    // Re-schedule the timer.
    m_timer_id = m_timer_manager->AddOneshotTimer(
        FLAGS_toft_counter_output_interval_in_s * 1000,
        NewClosure(this, &CounterManager::ReportCounterWrapper));
}

void CounterManager::ReportCounterWrapper(uint64_t timer_id) {
    m_thread_pool->AddTask(NewClosure(this, &CounterManager::ReportCounter));
}

