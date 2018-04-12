#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMIC_H_INCLUDED
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMIC_H_INCLUDED

/*

Function style interface

///////////////////////////////////////////////////////////////////////////////
// return value
AtomicGet

///////////////////////////////////////////////////////////////////////////////
// Change value and return
AtomicSet
AtomicAdd
AtomicSub
AtomicAnd
AtomicOr
AtomicXor

T AtomicExchange<Operation>(T& target, T value)
Operation:
    atomically
    {
        target operation value;
        return target;
    }

///////////////////////////////////////////////////////////////////////////////
// change value and return old value

AtomicExchangeSet
AtomicExchangeAdd
AtomicExchangeSub
AtomicExchangeAnd
AtomicExchangeOr
AtomicExchangeXor

Prototype:
    T AtomicExchange<Operation>(T& target, T value)

Operation:
    atomically
    {
        T old = target;
        target operation value;
        return old;
    }


///////////////////////////////////////////////////////////////////////////////
// compare and change

Prototype:
    bool AtomicCompareExchange(T& value, T compare, T exchange, T& old)

Operation:
    atomically
    {
        old = value;
        if (value == compare)
        {
            value = exchange;
            return true;
        }
        return false;
    }

*/

// import implementation for each platform
#ifdef __GNUC__
#include "toft/system/atomic/atomic_asm/AtomicGcc.h"
#else
#error unsupported compiler
#endif

// convert implementation to global namespace
#include "toft/system/atomic/atomic_asm/AtomicAdapter.h"

#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMIC_H_INCLUDED
