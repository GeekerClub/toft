// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "thirdparty/gtest/gtest.h"
#include "toft/crypto/random/true_random.h"

// namespace common {

TEST(TrueRandom, NextUInt32)
{
    TrueRandom tr;
    double total = 0.0;
    for (int i = 0; i < 1000; ++i)
    {
        uint32_t n = tr.NextUInt32();
        EXPECT_GE(n, 0U);
        EXPECT_LE(n, UINT32_MAX);
        total += n;
    }
    EXPECT_NEAR(total / 1000 / UINT32_MAX, 0.5, 0.05);
}

TEST(TrueRandom, NextUInt32WithRange)
{
    TrueRandom tr;
    uint32_t max = 10000;
    double total = 0.0;
    for (int i = 0; i < 1000; ++i)
    {
        uint32_t n = tr.NextUInt32(max);
        EXPECT_GE(n, 0U);
        EXPECT_LT(n, max);
        total += n;
    }
    EXPECT_NEAR(total / 1000 / max, 0.5, 0.05);
}

TEST(TrueRandom, NextDouble)
{
    TrueRandom tr;
    double total = 0.0;
    for (int i = 0; i < 1000; ++i)
    {
        double n = tr.NextDouble();
        EXPECT_GE(n, 0);
        EXPECT_LE(n, 1.0);
        total += n;
    }
    EXPECT_NEAR(total / 1000, 0.5, 0.05);
}

// } // namespace common
