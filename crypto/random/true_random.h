// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_CRYPTO_RANDOM_TRUE_RANDOM_H
#define COMMON_CRYPTO_RANDOM_TRUE_RANDOM_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "toft/base/uncopyable.h"

// namespace common {

/// true random generator
class TrueRandom
{
public:
    TrueRandom();
    ~TrueRandom();

    /// return random integer in range [0, UINT_MAX]
    uint32_t NextUInt32();

    /// return random integer in range [0, max_value)
    uint32_t NextUInt32(uint32_t max_value);

    /// return double in range [0.0,1.0]
    double NextDouble();

    /// generate random bytes
    bool NextBytes(void* buffer, size_t size);

private:
    union
    {
        int m_fd;               /// fd for /dev/urandom
        intptr_t m_hcrypt_prov; /// HCRYPTPROV
    };
};

// } // namespace common

#endif // COMMON_CRYPTO_RANDOM_TRUE_RANDOM_H

