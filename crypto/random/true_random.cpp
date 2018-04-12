// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "toft/crypto/random/true_random.h"
#include <limits.h>
#include <stdlib.h>

#if __unix__
#include <fcntl.h>
#include <unistd.h>
#elif _WIN32
#include <toft/base/common_windows.h>
#include <wincrypt.h>
#else
#error Unknown platform
#endif

#ifdef _MSC_VER
#pragma comment(lib, "advapi32")
#endif

// namespace common {

#ifdef __unix__
TrueRandom::TrueRandom()
{
    m_fd = open("/dev/urandom", O_RDONLY, 0);
    if (m_fd < 0)
    {
        abort();
    }
}

TrueRandom::~TrueRandom()
{
    close(m_fd);
    m_fd = -1;
}

bool TrueRandom::NextBytes(void* buffer, size_t size)
{
    return read(m_fd, buffer, size) == static_cast<int>(size);
}

#elif _WIN32
TrueRandom::TrueRandom()
{
    HCRYPTPROV hcrypt_prov;
    if (CryptAcquireContext(&hcrypt_prov, NULL, NULL, PROV_RSA_FULL, 0))
    {
        m_hcrypt_prov = hcrypt_prov;
    }
    else
    {
        abort();
    }
}

TrueRandom::~TrueRandom()
{
    CryptReleaseContext(m_hcrypt_prov, 0);
    m_hcrypt_prov = 0;
}

bool TrueRandom::NextBytes(void* buffer, size_t size)
{
    return CryptGenRandom(m_hcrypt_prov, static_cast<DWORD>(size), static_cast<BYTE*>(buffer))
        != FALSE;
}
#endif

uint32_t TrueRandom::NextUInt32()
{
    uint32_t bytes = -1;
    NextBytes(&bytes, sizeof(bytes));
    return bytes;
}

uint32_t TrueRandom::NextUInt32(uint32_t max_value)
{
    return NextUInt32() % max_value;
}

double TrueRandom::NextDouble()
{
    uint32_t n = NextUInt32();
    return static_cast<double>(n) / UINT32_MAX;
}

// } // namespace common
