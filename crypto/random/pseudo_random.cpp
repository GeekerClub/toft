// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "toft/crypto/random/pseudo_random.h"
#include <limits.h>

// namespace common {

PseudoRandom::PseudoRandom(uint64_t seed)
    : m_seed(seed)
{
}

void PseudoRandom::SetSeed(uint64_t seed)
{
    m_seed = seed;
}

uint32_t PseudoRandom::NextUInt32()
{
    m_seed = (m_seed * 0x5deece66dULL + 11) & 0xffffffffffffULL;
    return static_cast<uint32_t>(m_seed >> 16);
}

uint32_t PseudoRandom::NextUInt32(uint32_t max_value)
{
    return NextUInt32() % max_value;
}

double PseudoRandom::NextDouble()
{
    return NextUInt32() / static_cast<double>(UINT32_MAX);
}

void PseudoRandom::NextBytes(void* buffer, size_t size)
{
    unsigned char* p = reinterpret_cast<unsigned char*>(buffer);
    for (size_t i = 0; i < size; ++i)
        p[i] = NextUInt32() & UCHAR_MAX;
}

// } // namespace common
