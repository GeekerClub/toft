// Copyright 2013, For authors. All rights reserved.
// Author: An Qin (anqin.qin@gmail.com)
//
#include "toft/container/blocking_queue.hpp"

#include <queue>
#include <string>

#include "gtest/gtest.h"
#include "toft/base/scoped_ptr.h"
#include "toft/system/threading/thread.h"

namespace toft {

class BlockingQueueTest : public testing::Test {
protected:
    BlockingQueueTest() {}

    static void PushValueOnceFn(
            BlockingQueue<std::string>* blocking_queue,
            const std::string* str) {
        blocking_queue->Push(*str);
    }


    BlockingQueue<std::string>   m_blocking_queue;
};

TEST_F(BlockingQueueTest, TestStringQueue) {
    std::string  push_str = "just for test!!!";
    for (int i = 0; i < 5; ++i) {
        this->m_blocking_queue.Push(push_str);
    }

    for (int i = 0; i < 5; ++i) {
        std::string  pop_str;
        this->m_blocking_queue.Pop(&pop_str);
        EXPECT_EQ(pop_str, push_str);
    }
}

TEST_F(BlockingQueueTest, TestFrontAndBack) {
    std::string  element;
    EXPECT_FALSE(this->m_blocking_queue.Front(&element));
    EXPECT_FALSE(this->m_blocking_queue.Back(&element));

    std::string  push_str = "just for test!!!";
    this->m_blocking_queue.Push(push_str);

    EXPECT_TRUE(this->m_blocking_queue.Front(&element));
    EXPECT_EQ(push_str, element);

    EXPECT_TRUE(this->m_blocking_queue.Back(&element));
    EXPECT_EQ(push_str, element);

    std::string  pop_str;
    this->m_blocking_queue.Pop(&pop_str);
    EXPECT_EQ(pop_str, push_str);

    EXPECT_FALSE(this->m_blocking_queue.Front(&element));
    EXPECT_FALSE(this->m_blocking_queue.Back(&element));
}

TEST_F(BlockingQueueTest, TestBlockingQueue) {
    std::string  pop_str;
    // pop element for empty blocking queue.
    EXPECT_FALSE(this->m_blocking_queue.Pop(&pop_str, 2));

    std::string  push_str = "just for test !!!!";
    Thread thread;
    thread.Start(std::bind(
            BlockingQueueTest::PushValueOnceFn,
            &m_blocking_queue, &push_str));
    thread.Join();

    EXPECT_TRUE(this->m_blocking_queue.Pop(&pop_str, 2));
    EXPECT_EQ(pop_str, push_str);
}

}  // namespace toft
