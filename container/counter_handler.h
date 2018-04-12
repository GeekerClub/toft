// Copyright (C) 2016, For authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:

#ifndef TOFT_CONTAINER_COUNTER_HANDLER_H
#define TOFT_CONTAINER_COUNTER_HANDLER_H

#include <list>
#include <string>

#include "toft/base/closure.h"

namespace toft {

class CounterHandler {
public:
    struct RateRecord {
        RateRecord(const std::string& name, int64_t count)
            : m_name(name), m_count(count) {}
        std::string m_name;
        int64_t m_count;
    };

    struct ValueRecord {
        ValueRecord(const std::string& name,
                    int64_t min, int64_t max, int64_t avg)
            : m_name(name),
              m_min(min),
              m_max(max),
              m_avg(avg) {}
        std::string m_name;
        int64_t m_min;
        int64_t m_max;
        int64_t m_avg;
    };

    struct MapRecord {
        MapRecord(const std::string& name,
                  const std::string& key,
                  int64_t value)
            : m_name(name), m_key(key), m_value(value) {}
        std::string m_name;
        std::string m_key;
        int64_t m_value;
    };

    struct SumRecord {
        SumRecord(const std::string& name, int64_t count)
            : m_name(name), m_count(count) {}
        std::string m_name;
        int64_t m_count;
    };

    virtual ~CounterHandler() {}

    virtual bool HandleData(const std::list<RateRecord>& rate_records,
                            const std::list<ValueRecord>& value_records,
                            const std::list<MapRecord>& map_records,
                            const std::list<SumRecord>& sum_records) {
        return true;
    }

};

typedef Closure<void (
        const std::list<CounterHandler::RateRecord>&,
        const std::list<CounterHandler::ValueRecord>&,
        const std::list<CounterHandler::MapRecord>&,
        const std::list<CounterHandler::SumRecord>&) > CounterCallback;

} // namespace toft

#endif // TOFT_CONTAINER_COUNTER_HANDLER_H
