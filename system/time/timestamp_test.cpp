// Copyright 2013, For toft authors.
//
// Author: An Qin (anqin.qin@gmail.com)


#include "toft/system/time/timestamp.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

const int kLoopCount = 1000000;

TEST(Timestamp, GetTimestampInUs) {
    for (int i = 0; i < kLoopCount; ++i) {
        GetTimestampInUs();
    }
}

TEST(Timestamp, GetTimestampInMs) {
    int64_t t0 = GetTimestampInMs();
    for (int64_t i = 0; i < kLoopCount; ++i) {
        int64_t t = GetTimestampInMs();
        int diff = t - t0;
        if (diff < 0) {
            if (diff < -5)
                printf("time backward %d ms\n", diff);
        }
        t0 = t;
    }
}


} // namespace toft
