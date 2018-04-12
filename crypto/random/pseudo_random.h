// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_CRYPTO_RANDOM_PSEUDO_RANDOM_H
#define COMMON_CRYPTO_RANDOM_PSEUDO_RANDOM_H

#include <stdint.h>
#include <stddef.h>

// namespace common {

/// 伪随机数发生器，线性同余算法
class PseudoRandom
{
    /// can be copied safety
public:
    explicit PseudoRandom(uint64_t seed);

    /// return random integer between 0 ~ UINT32_MAX
    uint32_t NextUInt32();

    /** Returns the next random number, limited to a given range.
        @returns a random integer between 0 (inclusive) and maxValue (exclusive).
    */
    uint32_t NextUInt32(uint32_t max_value);

    /** Returns the next random floating-point number.
        @returns a random value in the range 0 to 1.0
    */
    double NextDouble();

    /** Resets this PseudoRandom object to a given seed value. */
    void SetSeed(uint64_t seed);

    /// 生成随机字节序列
    void NextBytes(void* buffer, size_t size);
private:
    uint64_t m_seed;
};

// } // namespace common

#endif // COMMON_CRYPTO_RANDOM_PSEUDO_RANDOM_H

