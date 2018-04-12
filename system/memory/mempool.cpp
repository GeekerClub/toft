// Copyright (c) 2012, Tencent Inc.
// All rights reserved.

#include "toft/system/memory/mempool.h"

#include <assert.h>
#include <stdio.h>
#include <algorithm>

#include "toft/base/array_size.h"
#include "toft/system/atomic/atomic.h"


namespace toft {

const size_t MemPool::MIN_UNIT_SIZE;
const size_t MemPool::MAX_UNIT_SIZE;
const int MemPool::MAX_INDEX;

struct MemPool::BLOCK_HEADER
{
    unsigned char Magic[2]; // magic number to check, must be 'MP'
    short Index;            // 内存单元索引，-1 表示不在 cache 里
    unsigned int Size;      // 单元的大小
};

namespace {
struct BlockConfig
{
    size_t size;
    unsigned int cache_count;
    bool operator<(const BlockConfig& other) const
    {
        return size < other.size;
    }
};

static BlockConfig s_BlockConfig[] =
{
    { MemPool::MIN_UNIT_SIZE, 50 },
    { 2 * MemPool::MIN_UNIT_SIZE, 50 },
    { 4 * MemPool::MIN_UNIT_SIZE, 50 },
    { 8 * MemPool::MIN_UNIT_SIZE, 50 },
    { 16 * MemPool::MIN_UNIT_SIZE, 50 },
    { 32 * MemPool::MIN_UNIT_SIZE, 50 },
    { 64 * MemPool::MIN_UNIT_SIZE, 50 },
    { 128 * MemPool::MIN_UNIT_SIZE, 8 },
    { 256 * MemPool::MIN_UNIT_SIZE, 4 },
    { 512 * MemPool::MIN_UNIT_SIZE, 2 },
    { 1024 * MemPool::MIN_UNIT_SIZE, 1 },
    { 2048 * MemPool::MIN_UNIT_SIZE, 1 },
    { 4096 * MemPool::MIN_UNIT_SIZE, 1 },
    { 8192 * MemPool::MIN_UNIT_SIZE, 1 },
    { 16384 * MemPool::MIN_UNIT_SIZE, 1 },
    { 32768 * MemPool::MIN_UNIT_SIZE, 1 },
};
}

MemPool::MemPool(size_t dwMaxMemSize):
    m_poolMutex(),
    m_dwQueueNum(MAX_INDEX),
    m_allocated_memory_size(0),
    m_pooled_memory_size(0),
    m_dwAllocateCount(0),
    m_dwFreeCount(0),
    m_dwNewCount(0),
    m_dwDeleteCount(0)
{
    assert(dwMaxMemSize >= MIN_UNIT_SIZE);
    assert((dwMaxMemSize % MIN_UNIT_SIZE) == 0);
    assert(dwMaxMemSize <= MAX_UNIT_SIZE);
}

MemPool::~MemPool()
{
    //////////////////////////////////////////////////////////////////////////
    // 释放MemPool中的内存;
    BLOCK_HEADER* pBlock = NULL;

    for (unsigned int i = 0; i < m_dwQueueNum; i++)
    {
        while (!(m_memPool[i].empty()))
        {
            pBlock = m_memPool[i].front();

            if (pBlock != NULL)
            {
                FreeBlock(pBlock);
            }

            m_memPool[i].pop_front();
        }
    }
}

bool MemPool::IsValidMemoryBlock(const MemPool::BLOCK_HEADER* block_header)
{
    if (block_header->Magic[0] != 'M' || block_header->Magic[1] != 'P')
        return false;

    if (block_header->Index == -1)
        return block_header->Size > MAX_UNIT_SIZE;

    return block_header->Index < MAX_INDEX &&
        block_header->Size <= MAX_UNIT_SIZE;
}

MemPool::BLOCK_HEADER* MemPool::AllocateBlock(unsigned int index)
{
    size_t size = sizeof(BLOCK_HEADER) + s_BlockConfig[index].size;
    BLOCK_HEADER* p = static_cast<BLOCK_HEADER*>(operator new(size));
    p->Magic[0] = 'M';
    p->Magic[1] = 'P';
    p->Index = index;
    p->Size = s_BlockConfig[index].size;
    AtomicIncrement(&m_dwNewCount);
    return p;
}

MemPool::BLOCK_HEADER* MemPool::AllocateLargeBlock(size_t size)
{
    size_t new_size = sizeof(BLOCK_HEADER) + size;
    BLOCK_HEADER* p = static_cast<BLOCK_HEADER*>(operator new(new_size));
    p->Magic[0] = 'M';
    p->Magic[1] = 'P';
    p->Index = -1;
    p->Size = size;
    AtomicIncrement(&m_dwNewCount);
    return p;
}

void MemPool::FreeBlock(BLOCK_HEADER* block)
{
    operator delete(block);
    AtomicIncrement(&m_dwDeleteCount);
}

//////////////////////////////////////////////////////////////////////////
// 功能描述:             分配一个大小为dwSize的MemUnit
// 输入参数:             @参数1: dwSize; MemUnit的大小;
// 返回值:               MemUnit的指针;
void* MemPool::Allocate(size_t dwSize)
{
    BLOCK_HEADER* pBlock = NULL;
    if (dwSize != 0)
    {
        int dwArrayIndex = GetUnitIndex(dwSize);

        if (dwArrayIndex >= MAX_INDEX)
        {
            pBlock = AllocateLargeBlock(dwSize);
        }
        else
        {
            {
                SpinLock::Locker locker(&m_poolMutex);

                if (!m_memPool[dwArrayIndex].empty())
                {
                    m_pooled_memory_size -= s_BlockConfig[dwArrayIndex].size;
                    pBlock = m_memPool[dwArrayIndex].front();
                    m_memPool[dwArrayIndex].pop_front();
                }
            }

            if (!pBlock)
            {
                pBlock = AllocateBlock(dwArrayIndex);
            }
        }
    }

    if (pBlock)
    {
        SpinLock::Locker locker(&m_poolMutex);
        m_dwAllocateCount++;
        m_allocated_memory_size += pBlock->Size;
        return pBlock + 1;
    }

    return NULL;
}

// 功能描述:                释放一个MemUnit的资源
// 输入参数:                @参数1: pUnit, 内存块的指针;
// 返回值:              无
void MemPool::Free(void* p)
{
    if (p)
    {
        BLOCK_HEADER* pBlock = reinterpret_cast<BLOCK_HEADER*>(p) - 1;
        assert(IsValidMemoryBlock(pBlock));

        int dwArrayIndex = pBlock->Index;
        size_t size = pBlock->Size;

        if (dwArrayIndex == -1)
        {
            FreeBlock(pBlock);
        }
        else
        {
            {
                SpinLock::Locker locker(&m_poolMutex);
                size_t dwCurQueueSize = m_memPool[dwArrayIndex].size();
                if (dwCurQueueSize < s_BlockConfig[dwArrayIndex].cache_count)
                {
                    m_memPool[dwArrayIndex].push_back(pBlock);
                    m_pooled_memory_size += s_BlockConfig[dwArrayIndex].size;
                    pBlock = NULL;
                }
            }

            if (pBlock)
            {
                FreeBlock(pBlock);
            }
        }

        {
            SpinLock::Locker locker(&m_poolMutex);
            ++m_dwFreeCount;
            m_allocated_memory_size -= size;
        }
    }
}

size_t MemPool::GetBlockSize(const void* ptr) const
{
    if (ptr)
    {
        const BLOCK_HEADER* pBlock = reinterpret_cast<const BLOCK_HEADER*>(ptr) - 1;
        return pBlock->Size;
    }
    return 0;
}

size_t MemPool::GetAllocatedSize() const
{
    return m_allocated_memory_size;
}

size_t MemPool::GetPooledSize() const
{
    return m_pooled_memory_size;
}

// 功能描述:                获取内存块的循环Buffer索引编号;
// 输入参数:                @参数1: dwSize, 内存块的大小;
// 返回值:              索引编号值;
int MemPool::GetUnitIndex(size_t dwSize)
{
    BlockConfig block = { dwSize, 0 };
    BlockConfig* p = std::lower_bound(s_BlockConfig,
                                      s_BlockConfig + TOFT_ARRAY_SIZE(s_BlockConfig),
                                      block);
    return p - s_BlockConfig;
}

static int s_initialize_count;
static MemPool* s_pMemPool = NULL;

/// 初始化MemPool
int MemPool_Initialize(size_t dwMaxMemSize)
{
    int count = AtomicIncrement(&s_initialize_count);
    if (count == 1)
    {
        if (s_pMemPool == NULL)
            s_pMemPool = new MemPool(dwMaxMemSize);
    }
    else // TODO(phongchen): wait for s_pMemPool initialize complete
    {
    }

    return count;
}

/// 释放MemPool
int MemPool_Destroy()
{
    int count = AtomicDecrement(&s_initialize_count);
    if (count < 0)
    {
        fprintf(stderr, "Fatal error: unbalance Initialize/Destroy");
        abort();
    }

    if (count == 0)
    {
        delete s_pMemPool;
        s_pMemPool = NULL;
    }
    return count;
}

/// 线程安全的分配内存资源
void* MemPool_Allocate(size_t dwSize)
{
    assert(s_pMemPool != NULL);
    return s_pMemPool->Allocate(dwSize);
}

void MemPool_Free(void* p)
{
    assert(s_pMemPool);
    s_pMemPool->Free(p);
}

size_t MemPool_GetAllocatedSize()
{
    assert(s_pMemPool);
    return s_pMemPool->GetAllocatedSize();
}

size_t MemPool_GetPooledSize()
{
    assert(s_pMemPool);
    return s_pMemPool->GetPooledSize();
}

size_t MemPool_GetBlockSize(const void* ptr)
{
    assert(s_pMemPool);
    return s_pMemPool->GetBlockSize(ptr);
}

} // namespace toft
