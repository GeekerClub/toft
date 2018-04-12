// Copyright (c) 2012, Tencent Inc.
// All rights reserved.

#ifndef TOFT_SYSTEM_MEMORY_MEMPOOL_H
#define TOFT_SYSTEM_MEMORY_MEMPOOL_H

#include <assert.h>
#include <deque>
// #include "common/base/deprecate.h"
#include "toft/system/threading/spinlock.h"

namespace toft {

class MemPool
{
    struct BLOCK_HEADER;
public:
    static const size_t MIN_UNIT_SIZE = 1024;
    static const size_t MAX_UNIT_SIZE = 16 * 2048 * MIN_UNIT_SIZE;
    static const int MAX_INDEX = 16;

public:
    explicit MemPool(size_t dwMaxMemSize);

    ~MemPool();

    // 功能描述:             分配一个大小为dwSize的MemUnit
    // 输入参数:             @参数1: dwSize; MemUnit的大小;
    // 返回值:               MemUnit的指针;
    void* Allocate(size_t dwSize);

    // 功能描述:             释放一个MemUnit的资源
    // 输入参数:             @参数1: pUnit, 内存块的指针;
    // 返回值:               无
    void Free(void* p);

    size_t GetBlockSize(const void* ptr) const;
    size_t GetAllocatedSize() const;
    size_t GetPooledSize() const;

private:
    BLOCK_HEADER* AllocateBlock(unsigned int index);
    BLOCK_HEADER* AllocateLargeBlock(size_t size);
    void FreeBlock(BLOCK_HEADER* block);
    bool IsValidMemoryBlock(const BLOCK_HEADER* block_header);
    int GetUnitIndex(size_t dwSize);
private:
    SpinLock                m_poolMutex;
    std::deque<BLOCK_HEADER*>   m_memPool[MAX_INDEX];
    size_t m_dwQueueNum;
    size_t m_allocated_memory_size;
    size_t m_pooled_memory_size;
    size_t m_dwAllocateCount;
    size_t m_dwFreeCount;
    size_t m_dwNewCount;
    size_t m_dwDeleteCount;
};

// DEPRECATED_BY(MemPool::MIN_UNIT_SIZE)
const size_t MIN_MEMUNIT_SIZE = MemPool::MIN_UNIT_SIZE;

// DEPRECATED_BY(MemPool::MAX_UNIT_SIZE)
const size_t MAX_MEMUNIT_SIZE = MemPool::MAX_UNIT_SIZE;

// DEPRECATED_BY(MemPool::MAX_INDEX_NUM)
const int MAX_INDEX_NUM = MemPool::MAX_INDEX;

//////////////////////////////////////////////////////////////////////////
// 初始化 MemPool 默认实例
// 返回：第几次被初始化
// 注意：只有第一次时，参数才会真正被用到。
int MemPool_Initialize(size_t dwMaxMemSize = MemPool::MAX_UNIT_SIZE);

//////////////////////////////////////////////////////////////////////////
// 释放 MemPool 实例
// 返回: 0 释放成功
//       >0 因先前的多次初始化而没有未释放
int MemPool_Destroy();

//////////////////////////////////////////////////////////////////////////
// 线程安全的分配内存资源
void* MemPool_Allocate(size_t dwSize);
void MemPool_Free(void* p);

size_t MemPool_GetAllocatedSize();
size_t MemPool_GetPooledSize();

size_t MemPool_GetBlockSize(const void* ptr);

} // namespace toft

#endif // TOFT_SYSTEM_MEMORY_MEMPOOL_H

