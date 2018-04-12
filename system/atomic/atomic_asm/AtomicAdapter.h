#ifndef ATOMIC_ASM_ADAPTER_H_INCLUDED
#define ATOMIC_ASM_ADAPTER_H_INCLUDED

// os independed common implement forward to asm implemention

// Get
template <typename T>
inline T AtomicGet(const volatile T* target)
{
    return AtomicImplementation::Get(target);
}

// Set
template <typename T>
inline void AtomicSet(volatile T* target, T value)
{
    AtomicImplementation::Set(target, value);
}

template <typename T>
inline T AtomicExchange(volatile T* target, T value)
{
    return AtomicImplementation::Exchange(target, value);
}

template <typename T>
inline T AtomicExchangeAdd(volatile T* target, T value)
{
    return AtomicImplementation::ExchangeAdd(target, value);
}

template <typename T>
inline T AtomicExchangeSub(volatile T* target, T value)
{
    return AtomicImplementation::ExchangeSub(target, value);
}

template <typename T>
inline T AtomicExchangeAnd(volatile T* target, T value)
{
    return AtomicImplementation::ExchangeAnd(target, value);
}

template <typename T>
inline T AtomicExchangeOr(volatile T* target, T value)
{
    return AtomicImplementation::ExchangeOr(target, value);
}

template <typename T>
inline T AtomicExchangeXor(volatile T* target, T value)
{
    return AtomicImplementation::ExchangeXor(target, value);
}

template <typename T>
inline T AtomicAdd(volatile T* target, T value)
{
    return AtomicImplementation::Add(target, value);
}

template <typename T>
inline T AtomicSub(volatile T* target, T value)
{
    return AtomicImplementation::Sub(target, value);
}

template <typename T>
inline T AtomicIncrement(volatile T* target)
{
    return AtomicImplementation::Increment(target);
}

template <typename T>
inline T AtomicDecrement(volatile T* target)
{
    return AtomicImplementation::Decrement(target);
}

template <typename T>
inline T AtomicAnd(volatile T* target, T value)
{
    return AtomicImplementation::And(target, value);
}

template <typename T>
inline T AtomicOr(volatile T* target, T value)
{
    return AtomicImplementation::Or(target, value);
}

template <typename T>
inline T AtomicXor(volatile T* target, T value)
{
    return AtomicImplementation::Xor(target, value);
}

template <typename T>
inline bool AtomicCompareExchange(volatile T* target, T compare, T exchange, T* old)
{
    return AtomicImplementation::CompareExchange(target, compare, exchange, old);
}

template <typename T>
inline bool AtomicCompareExchange(volatile T* target, T compare, T exchange)
{
    T old;
    return AtomicImplementation::CompareExchange(target, compare, exchange, &old);
}

#endif // ATOMIC_ASM_ADAPTER_H_INCLUDED
