// All rights reserved.
//
// Author: CHEN Feng <phongchen@gmail.com>

#ifndef  TOFT_BASE_OBJECT_POOL_H_
#define  TOFT_BASE_OBJECT_POOL_H_

#include <limits.h>
#include <algorithm>
#include <vector>

#include "toft/system/threading/mutex.h"
#include "toft/system/threading/spinlock.h"

#undef min
#undef max

namespace toft {

struct ObjectPool_DefaultAllocator
{
    template <typename T>
    static T* New(T* hint)
    {
        return new T();
    }

    template <typename T>
    static void Clear(T* p)
    {
    }

    template <typename T>
    static void Delete(T* p)
    {
        delete p;
    }
};

/// 清空方式：调用 Clear 成员函数
struct ObjectPool_NormalAllocator : public ObjectPool_DefaultAllocator
{
    template <typename T>
    static void Clear(T* p)
    {
        p->Clear();
    }
};

/// 清空方式：调用 clear 成员函数，适用于各种 STL 容器
struct ObjectPool_StdCxxAllocator : public ObjectPool_DefaultAllocator
{
    template <typename T>
    static void Clear(T* p)
    {
        p->clear();
    }
};

/// 支持动态设置配额的对象池
template <typename T,
    typename Allocator = ObjectPool_DefaultAllocator,
    typename LockType = toft::SpinLock
>
class ObjectPool
{
    typedef ObjectPool ThisType;
public:
    typedef T ObjectType;
public:
    explicit ObjectPool(
        size_t quota = INT_MAX,  ///< 最大缓存的配额
        size_t initial_size = 0, ///< 初始大小
        bool auto_create = true, ///< 池为空时请求对象是否自动创建
        Allocator allocator = Allocator()
    ):
        m_Quota(quota), m_AutoCreate(auto_create), m_Allocator(allocator)
    {
        Reserve(std::min(initial_size, quota));
    }

    ~ObjectPool()
    {
        Clear();
    }

    /// 返回配额
    size_t GetQuota() const
    {
        return m_Quota;
    }

    size_t Size() const
    {
        typename LockType::Locker locker(&m_Lock);
        return m_PooledObjects.size();
    }

    /// 设置配额
    void SetQuota(size_t size)
    {
        typename LockType::Locker locker(&m_Lock);
        m_Quota = size;
        UnlockedShrink(size);
    }

    /// 预留对象
    /// @param size 期望预留的池中对象的个数
    void Reserve(size_t size)
    {
        if (size > m_Quota)
            size = m_Quota;
        typename LockType::Locker locker(&m_Lock);
        while (m_PooledObjects.size() < size)
        {
            m_PooledObjects.push_back(NewObject());
        }
    }

    /// 从对象池获得对象
    T* Acquire()
    {
        {
            typename LockType::Locker locker(&m_Lock);
            if (m_PooledObjects.empty())
            {
                if (!m_AutoCreate)
                    return NULL;
            }
            else
            {
                T* p = m_PooledObjects.back();
                m_PooledObjects.pop_back();
                return p;
            }
        }
        return NewObject();
    }

    /// 归还对象
    void Release(const T* p)
    {
        T* q = const_cast<T*>(p);
        m_Allocator.Clear(q);
        {
            typename LockType::Locker locker(&m_Lock);
            if (m_PooledObjects.size() < m_Quota)
            {
                m_PooledObjects.push_back(q);
                return;
            }
        }
        m_Allocator.Delete(q);
    }

    /// 收缩对象池里缓存的对象到不多于 size 个
    void Shrink(size_t size = 0)
    {
        typename LockType::Locker locker(&m_Lock);
        UnlockedShrink(size);
    }

    /// 清空所有缓存的对象
    void Clear()
    {
        Shrink(0);
    }
private:
    void UnlockedShrink(size_t size)
    {
        while (m_PooledObjects.size() > size)
        {
            delete m_PooledObjects.back();
            m_PooledObjects.pop_back();
        }
    }

    T* NewObject()
    {
        return m_Allocator.New(static_cast<T*>(0));
    }

private:
    ObjectPool(const ObjectPool& src);
    ObjectPool& operator=(const ObjectPool& rhs);
private:
    mutable LockType m_Lock;
    std::vector<T*> m_PooledObjects;
    size_t m_Quota;
    bool m_AutoCreate;
    Allocator m_Allocator;
};

/// @brief 编译期间指定配额的对象池
/// @tparam T 对象类型
/// @tparam Quota 设置的配额
/// @tparam Allocator 指定清空对象的方法
template <
    typename T,
    size_t Quota,
    typename Allocator = ObjectPool_DefaultAllocator,
    typename LockType = toft::SpinLock
>
class FixedObjectPool
{
    typedef FixedObjectPool ThisType;
public:
    typedef T ObjectType;
public:
    explicit FixedObjectPool(
        size_t initial_size = 0,
        bool auto_create = true,
        Allocator allocator = Allocator()
    ) :
        m_Count(0), m_AutoCreate(auto_create), m_Allocator(allocator)
    {
        Reserve(std::min(Quota, initial_size));
    }

    ~FixedObjectPool()
    {
        Clear();
    }

    /// 从对象池获得对象
    T* Acquire()
    {
        {
            typename LockType::Locker locker(&m_Lock);
            if (m_Count > 0)
            {
                return m_PooledObjects[--m_Count];
            }
        }
        if (m_AutoCreate)
            return NewObject();
        else
            return NULL;
    }

    /// 归还对象
    void Release(const T* p)
    {
        T* q = const_cast<T*>(p);
        m_Allocator.Clear(q);
        {
            typename LockType::Locker locker(&m_Lock);
            if (m_Count < Quota)
            {
                m_PooledObjects[m_Count++] = q;
                return;
            }
        }
        m_Allocator.Delete(q);
    }

    size_t Size() const
    {
        typename LockType::Locker locker(&m_Lock);
        return m_Count;
    }

    /// 预留对象
    /// @param size 期望预留的池中对象的个数
    void Reserve(size_t size)
    {
        if (size > Quota)
            size = Quota;
        typename LockType::Locker locker(&m_Lock);
        while (m_Count < size)
        {
            T* p = NewObject();
            m_PooledObjects[m_Count++] = p;
        }
    }

    /// 收缩缓存的对象
    void Shrink(size_t size = 0)
    {
        typename LockType::Locker locker(&m_Lock);
        UnlockedShrink(size);
    }

    /// 清除所有缓存的对象
    void Clear()
    {
        typename LockType::Locker locker(&m_Lock);
        UnlockedShrink(0);
    }

private:
    FixedObjectPool(const FixedObjectPool& src);
    FixedObjectPool& operator=(const FixedObjectPool& rhs);
    T* NewObject()
    {
        return m_Allocator.New(static_cast<T*>(0));
    }

    /// 收缩缓存的对象
    void UnlockedShrink(size_t size)
    {
        while (m_Count > size)
        {
            m_Allocator.Delete(m_PooledObjects[--m_Count]);
        }
    }

private:
    mutable LockType m_Lock;
    T* m_PooledObjects[Quota];
    size_t m_Count;
    bool m_AutoCreate;
    Allocator m_Allocator;
};

}  // namespace toft

#endif  // TOFT_BASE_OBJECT_POOL_H_
