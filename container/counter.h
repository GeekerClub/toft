// Copyright (C) 2016, For authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:


#ifndef TOFT_CONTAINER_COUNTER_H
#define TOFT_CONTAINER_COUNTER_H

#include <stdint.h>

#include <list>
#include <map>
#include <string>
#include <vector>

#include "toft/base/scoped_ptr.h"
#include "toft/system/threading/mutex.h"

#include "toft/container/counter_handler.h"

namespace toft {

class TimerManager;
class ThreadPool;

class RateCounter {
public:
    explicit RateCounter(const char* name, const char* description)
        : m_name(name), m_description(description) {}

    void AddCount(int count = 1) {
        MutexLocker lock(&m_mutex);
        m_value += count;
    }

    uint64_t GetValue() const {
        MutexLocker lock(&m_mutex);
        return m_value;
    }

    void GetAndReset(uint64_t *value) {
        MutexLocker lock(&m_mutex);
        *value = m_value;
        m_value = 0;
    }

private:
    friend class CounterManager;

    const char* const m_name;
    const char* const m_description;

    uint64_t m_value;
    mutable Mutex m_mutex;
};

class ValueCounter {
public:
    explicit ValueCounter(const char* name, const char* description)
        : m_name(name),
          m_description(description),
          m_has_data(false),
          m_count(0),
          m_sum(0),
          m_max(INT64_MIN),
          m_min(INT64_MAX) {}

    void SetValue(int value) {
        MutexLocker lock(&m_mutex);
        m_has_data = true;
        ++m_count;
        m_sum += value;
        m_min = m_min < value ? m_min : value;
        m_max = m_max > value ? m_max : value;
    }

    std::string GetName() const {
        return m_name;
    }

    void GetAndReset(int64_t* avg, int64_t* max, int64_t* min) {
        MutexLocker lock(&m_mutex);
        if (!m_has_data) {
            *avg = *max = *min = 0;
            return;
        }

        // Read data.
        *avg = m_sum / m_count;
        *max = m_max;
        *min = m_min;

        // Reset.
        m_has_data = false;
        m_sum = 0;
        m_count = 0;
        m_max = INT64_MIN;
        m_min = INT64_MAX;
    }

private:
    const char* const m_name;
    const char* const m_description;

    bool m_has_data;
    int64_t m_count;
    int64_t m_sum;
    int64_t m_max;
    int64_t m_min;
    mutable Mutex m_mutex;
};

// Counter for map type which collect key-value pairs.
class MapCounter {
public:
    explicit MapCounter(const char* name, const char* description)
        : m_name(name), m_description(description) {}

    void Insert(const std::string& key, int64_t value) {
        MutexLocker lock(&m_mutex);
        m_has_data = true;
        m_map[key] = value;
    }

    std::string GetName() const {
        return m_name;
    }

    void GetAndReset(std::map<std::string, int64_t>* map) {
        MutexLocker lock(&m_mutex);
        if (!m_has_data) {
            map->clear();
            return;
        }

        // Copy data.
        *map = m_map;

        // Reset.
        m_has_data = false;
        m_map.clear();
    }

private:
    friend class CounterManager;

    const char* const m_name;
    const char* const m_description;

    bool m_has_data;
    std::map<std::string, int64_t> m_map;
    mutable Mutex m_mutex;
};

class SumCounter {
public:
    explicit SumCounter(const char* name, const char* description)
        : m_name(name), m_description(description) {}

    void AddCount(int count = 1) {
        MutexLocker lock(&m_mutex);
        m_value += count;
    }

    uint64_t GetValue() const {
        MutexLocker lock(&m_mutex);
        return m_value;
    }

    void GetAndReset(uint64_t *value) {
        MutexLocker lock(&m_mutex);
        *value = m_value;
        m_value = 0;
    }

private:
    friend class CounterManager;

    const char* const m_name;
    const char* const m_description;

    uint64_t m_value;
    mutable Mutex m_mutex;
};

enum CounterType {
    Counter_Type_Rate = 0,
    Counter_Type_Value = 1,
    Counter_Type_Map = 2,
    Counter_Type_Sum = 3
};

// CounterPointer stores the pointer to global static counter address.
class CounterPointer {
public:
    CounterPointer(CounterType type, void* valbuf)
        : m_type(type) {
        if (m_type == Counter_Type_Rate) {
            m_rate_counter = static_cast<RateCounter*>(valbuf);
        } else if (m_type == Counter_Type_Value) {
            m_value_counter = static_cast<ValueCounter*>(valbuf);
        } else if (m_type == Counter_Type_Map) {
            m_map_counter = static_cast<MapCounter*>(valbuf);
        } else {
            m_sum_counter = static_cast<SumCounter*>(valbuf);
        }
    }
    // Never delete the pointer in destructor.
    ~CounterPointer() {}

private:
    friend class CounterManager;

    CounterType m_type;

    union {
        RateCounter* m_rate_counter;
        ValueCounter* m_value_counter;
        MapCounter* m_map_counter;
        SumCounter* m_sum_counter;
    };
};

class CounterManager {
public:
    static CounterManager* GlobalInstance() {
        static CounterManager s_counter_manager;
        return &s_counter_manager;
    }

    CounterManager()
        : m_timer_manager(NULL),
          m_thread_pool(NULL),
          m_timer_id(0) {}

    // CounterManager owns the outputter.
    void Init(TimerManager* timer_manager, ThreadPool* thread_pool,
              CounterHandler* counter_handler);
    void Init(TimerManager* timer_manager, ThreadPool* thread_pool,
              std::vector<CounterHandler*>& counter_handlers);
    void Init(TimerManager* timer_manager, ThreadPool* thread_pool,
              CounterCallback* counter_callback);
    void Init(TimerManager* timer_manager, ThreadPool* thread_pool,
              std::vector<CounterCallback*>& counter_callbacks);

    void Init(TimerManager* timer_manager, ThreadPool* thread_pool,
              std::vector<CounterHandler*>& counter_handlers,
              std::vector<CounterCallback*>& counter_callbacks);

    // Stop counter upload task.
    void Terminate();

    void RegisterCounter(CounterPointer counter) {
        MutexLocker locker(&m_mutex);
        m_counters.push_back(counter);
    }

private:
    void ReportCounter();
    void ReportCounterWrapper(uint64_t timer_id);
    void CleanHandlers();

    Mutex m_mutex;

    std::list<CounterPointer> m_counters;

    TimerManager* m_timer_manager;
    ThreadPool* m_thread_pool;
    std::vector<CounterHandler*> m_counter_handlers;
    std::vector<CounterCallback*> m_counter_callbacks;

    uint64_t m_timer_id;
};

// A help class for registering the counter.
class CounterRegisterer {
public:
    CounterRegisterer(const CounterType type, void* pointer) {
        CounterPointer counter(type, pointer);
        CounterManager::GlobalInstance()->RegisterCounter(counter);
    }
};

} // namespace toft

#define DEFINE_COUNTER(type, name, description) \
    namespace toft { \
    type##Counter COUNTER_##name(#name, #description); \
    static CounterRegisterer o_##name(Counter_Type_##type, &COUNTER_##name); \
} \
    using toft::COUNTER_##name

#define DECLARE_COUNTER(type, name) \
    namespace toft { \
    extern type##Counter COUNTER_##name; \
} \
    using toft::COUNTER_##name


using toft::Counter_Type_Rate;
using toft::Counter_Type_Value;
using toft::Counter_Type_Map;
using toft::RateCounter;
using toft::ValueCounter;
using toft::MapCounter;
using toft::SumCounter;
using toft::CounterRegisterer;
using toft::CounterManager;

#endif // TOFT_CONTAINER_COUNTER_H
