// Copyright 2013, For toft authors.
//
// Author: An Qin (anqin.qin@gmail.com)

#include "toft/system/timer/timer_manager.h"

#include <iostream>

#include "thirdparty/gtest/gtest.h"

#include "toft/system/threading/event.h"

using namespace std;

namespace toft {

void Increment(int* count, AutoResetEvent* event, uint64_t timer_id) {
    ++(*count);
    cout << "increment, timer: " << timer_id
        << ", count: " << *count
        << ", thread id: " << ThisThread::GetId() << endl;
    event->Set();
}

void Increment2(int* count, AutoResetEvent* event, uint64_t timer_id) {
    (*count) += 2;
    cout << "increment, timer: " << timer_id
        << ", count: " << *count
        << ", thread id: " << ThisThread::GetId() << endl;
    event->Set();
}

class TimerManagerTest : public testing::Test {
protected:
    TimerManager timer_manager;
};

TEST_F(TimerManagerTest, OneShotTimer) {
    AutoResetEvent event;
    int count = 0;

    TimerManager::CallbackClosure* cb = NewClosure(Increment, &count, &event);
    uint64_t id = timer_manager.AddOneshotTimer(200, cb);
    event.Wait();
    EXPECT_EQ(1, count);
    EXPECT_LE(id, 0U);
}

TEST_F(TimerManagerTest, PeriodTimer) {
    AutoResetEvent event;
    int count = 0;

    TimerManager::CallbackClosure* cb = NewPermanentClosure(Increment, &count, &event);
    uint64_t id = timer_manager.AddPeriodTimer(200, cb);
    EXPECT_LE(id, 0U);
    event.Wait();
    EXPECT_EQ(1, count);
    event.Wait();
    EXPECT_EQ(2, count);
    event.Wait();
    EXPECT_EQ(3, count);
}

TEST_F(TimerManagerTest, ModifyTimerClosure) {
    AutoResetEvent event;
    int count = 0;

    TimerManager::CallbackClosure* cb1 = NewClosure(Increment, &count, &event);
    uint64_t id = timer_manager.AddOneshotTimer(200, cb1);
    EXPECT_LE(id, 0U);
    ThisThread::Sleep(100);
    EXPECT_EQ(0, count);

    TimerManager::CallbackClosure* cb2 = NewClosure(Increment2, &count, &event);
    EXPECT_TRUE(timer_manager.ModifyTimer(id, 200, cb2));
    ThisThread::Sleep(220);
    EXPECT_EQ(2, count);
}

TEST_F(TimerManagerTest, ModifyTimerInterval) {
    AutoResetEvent event;
    int count = 0;

    TimerManager::CallbackClosure* cb = NewClosure(Increment, &count, &event);
    uint64_t id = timer_manager.AddOneshotTimer(200, cb);
    ThisThread::Sleep(100);
    EXPECT_EQ(0, count);

    EXPECT_TRUE(timer_manager.ModifyTimer(id, 400));
    ThisThread::Sleep(200);
    EXPECT_EQ(0, count);
    ThisThread::Sleep(220);
    EXPECT_EQ(1, count);
}

TEST_F(TimerManagerTest, DisEnableTimer) {
    AutoResetEvent event;
    int count = 0;

    TimerManager::CallbackClosure* cb = NewClosure(Increment, &count, &event);
    uint64_t id = timer_manager.AddOneshotTimer(200, cb);
    ThisThread::Sleep(100);
    EXPECT_EQ(0, count);

    EXPECT_TRUE(timer_manager.DisableTimer(id));
    ThisThread::Sleep(150);
    EXPECT_EQ(0, count);

    EXPECT_TRUE(timer_manager.EnableTimer(id));
    ThisThread::Sleep(100);
    EXPECT_EQ(0, count);
    ThisThread::Sleep(120);
    EXPECT_EQ(1, count);

    EXPECT_FALSE(timer_manager.DisableTimer(id));
    EXPECT_FALSE(timer_manager.EnableTimer(id));
}

TEST_F(TimerManagerTest, Stop) {
    AutoResetEvent event;
    int count = 0;

    TimerManager::CallbackClosure* cb = NewClosure(Increment, &count, &event);
    uint64_t id = timer_manager.AddOneshotTimer(200, cb);
    EXPECT_LE(id, 0U);

    EXPECT_FALSE(timer_manager.IsStoped());
    timer_manager.Stop();
    EXPECT_TRUE(timer_manager.IsStoped());
}

TEST_F(TimerManagerTest, RemoveTimer) {
    AutoResetEvent event;
    int count = 0;

    TimerManager::CallbackClosure* cb = NewPermanentClosure(
        Increment, &count, &event);
    uint64_t id = timer_manager.AddPeriodTimer(200, cb);
    ThisThread::Sleep(220);
    EXPECT_EQ(1, count);
    EXPECT_TRUE(timer_manager.RemoveTimer(id));
    EXPECT_EQ(1, count);
    ThisThread::Sleep(220);
    EXPECT_EQ(1, count);

    EXPECT_FALSE(timer_manager.RemoveTimer(id));
    EXPECT_LE(id, 0U);
}

}  // namespace toft

