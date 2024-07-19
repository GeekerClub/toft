// Copyright (C) 2016, For authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:


#include "toft/container/counter.h"

#include "gflags/gflags.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "toft/container/counter_handler.h"
#include "toft/system/threading/event.h"
#include "toft/system/threading/this_thread.h"
#include "toft/system/threading/thread_pool.h"
#include "toft/system/timer/timer_manager.h"

DECLARE_bool(toft_counter_enabled);
DECLARE_int32(toft_counter_output_interval_in_s);

DEFINE_COUNTER(Rate, rate_counter, "test rate counter");
DEFINE_COUNTER(Value, value_counter, "test value counter");
DEFINE_COUNTER(Map, map_counter, "test map counter");


namespace toft {

AutoResetEvent sync_evnet;

class TestHandler : public CounterHandler {
public:
    TestHandler() : m_count(0) {}
    virtual bool SetupDispatcher() {
        return true;
    }
    virtual bool HandleData(
        const std::list<RateRecord>& rate_records,
        const std::list<ValueRecord>& value_records,
        const std::list<MapRecord>& map_records,
        const std::list<SumRecord>& sum_records) {

        LOG(INFO) << "Output counter";

        if (m_count == 0) {
            ++m_count;
            EXPECT_EQ(1u, rate_records.size());
            EXPECT_EQ(11, rate_records.front().m_count);
            EXPECT_EQ("rate_counter", rate_records.front().m_name);

            EXPECT_EQ(1u, value_records.size());
            EXPECT_EQ(123, value_records.front().m_min);
            EXPECT_EQ(789, value_records.front().m_max);
            EXPECT_EQ(456, value_records.front().m_avg);
            EXPECT_EQ("value_counter", value_records.front().m_name);

            EXPECT_EQ("test", map_records.front().m_key);
            EXPECT_EQ(1, map_records.front().m_value);
            EXPECT_EQ("map_counter", map_records.front().m_name);
            sync_evnet.Set();
        }
        return true;
    }
    virtual bool TeardownDispatcher() {
        return true;
    }

private:
    int m_count;
};

class CounterTest : public ::testing::Test {
    // do nothing
};

TEST_F(CounterTest, WriteCounter) {
    LOG(INFO) << "CounterTest start.";
    ::google::FlagSaver fs;

    FLAGS_toft_counter_output_interval_in_s = 1;
    FLAGS_toft_counter_enabled = true;
    ThreadPool thread_pool;
    TimerManager timer_manager;
    TestHandler test_handler;

//     if (timer_manager.IsStoped()) {
//         ThisThread::Sleep(3);
//     }
    CounterManager::GlobalInstance()->Init(
        &timer_manager,
        &thread_pool,
        &test_handler);

    COUNTER_rate_counter.AddCount();
    COUNTER_rate_counter.AddCount(10);
    COUNTER_value_counter.SetValue(123);
    COUNTER_value_counter.SetValue(456);
    COUNTER_value_counter.SetValue(789);
    COUNTER_map_counter.Insert("test", 1);

    ASSERT_TRUE(sync_evnet.TimedWait(10*1000)) << "Fail to wait dump counter.";
    CounterManager::GlobalInstance()->Terminate();
    LOG(INFO) << "CounterTest finish.";
}

} // namespace toft

int32_t main(int32_t argc, char* argv[]) {
    google::AllowCommandLineReparsing();
    google::InitGoogleLogging(argv[0]);
    if (!google::ParseCommandLineFlags(&argc, &argv, true))
        return -1;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

