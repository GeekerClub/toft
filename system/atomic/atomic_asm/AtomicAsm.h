#ifndef ATOMIC_ASM_H_INCLUDED
#define ATOMIC_ASM_H_INCLUDED

struct AtomicAsm
{
    template <typename T>
    static T Get(const volatile T* target)
    {
        return AtomicAsmWidth<sizeof(T)>::Get(target);
    }

    template <typename T>
    static void Set(volatile T* target, T value)
    {
        AtomicAsmWidth<sizeof(T)>::Set(target, value);
    }

    template <typename T>
    static T Exchange(volatile T* target, T value)
    {
        T old;
        AtomicAsmWidth<sizeof(T)>::Exchange(target, value, &old);
        return old;
    }

    template <typename T>
    static T ExchangeAdd(volatile T* target, T value)
    {
        return AtomicAsmWidth<sizeof(T)>::ExchangeAdd(target, value);
    }

    template <typename T>
    static T ExchangeSub(volatile T* target, T value)
    {
        return ExchangeAdd(target, static_cast<T>(-value));
    }

    static unsigned char ExchangeSub(volatile unsigned char* target, unsigned char value)
    {
        return static_cast<unsigned char>(
                   ExchangeSub(
                       reinterpret_cast<volatile signed char*>(target),
                       static_cast<signed char>(value)
                   )
               );
    }
    static unsigned short ExchangeSub(volatile unsigned short* target, unsigned short value)
    {
        return static_cast<unsigned short>(
                   ExchangeSub(
                       reinterpret_cast<volatile short*>(target),
                       static_cast<short>(value)
                   )
               );
    }
    static unsigned int ExchangeSub(volatile unsigned int* target, unsigned int value)
    {
        return static_cast<unsigned int>(
                   ExchangeSub(
                       reinterpret_cast<volatile int*>(target),
                       static_cast<int>(value)
                   )
               );
    }
    static unsigned long ExchangeSub(volatile unsigned long* target, unsigned long value)
    {
        return static_cast<unsigned long>(
                   ExchangeSub(
                       reinterpret_cast<volatile long*>(target),
                       static_cast<long>(value)
                   )
               );
    }
    static unsigned long long ExchangeSub(volatile unsigned long long* target, unsigned long long value)
    {
        return static_cast<unsigned long long>(
                   ExchangeSub(
                       reinterpret_cast<volatile long long*>(target),
                       static_cast<long long>(value)
                   )
               );
    }

    static float ExchangeAdd(volatile float* target, float value)
    {
        return AtomicCompareExchangeEmulated<AtomicAsmWidth<sizeof(float)> >::ExchangeAdd(target, value);
    }

    static double ExchangeAdd(volatile double* target, double value)
    {
        return AtomicCompareExchangeEmulated<AtomicAsmWidth<sizeof(double)> >::ExchangeAdd(target, value);
    }

    template <typename T>
    static T Add(volatile T* target, T value)
    {
        return ExchangeAdd(target, value) + value;
    }

    template <typename T>
    static T Sub(volatile T* target, T value)
    {
        return ExchangeSub(target, value) - value;
    }

    template <typename T>
    static T ExchangeAnd(volatile T* target, T value)
    {
        return AtomicAsmWidth<sizeof(T)>::ExchangeAnd(target, value);
    }

    template <typename T>
    static T ExchangeOr(volatile T* target, T value)
    {
        return AtomicAsmWidth<sizeof(T)>::ExchangeOr(target, value);
    }

    template <typename T>
    static T ExchangeXor(volatile T* target, T value)
    {
        return AtomicAsmWidth<sizeof(T)>::ExchangeXor(target, value);
    }

    template <typename T>
    static T And(volatile T* target, T value)
    {
        return ExchangeAnd(target, value) & value;
    }

    template <typename T>
    static T Or(volatile T* target, T value)
    {
        return ExchangeOr(target, value) | value;
    }

    template <typename T>
    static T Xor(volatile T* target, T value)
    {
        return ExchangeXor(target, value) ^ value;
    }

    template <typename T>
    static T Increment(volatile T* target)
    {
        return Add(target, static_cast<T>(1));
    }

    template <typename T>
    static T Decrement(volatile T* target)
    {
        return Sub(target, static_cast<T>(1));
    }

    template <typename T>
    static bool CompareExchange(volatile T* target, T compare, T exchange, T* old)
    {
        return AtomicAsmWidth<sizeof(T)>::CompareExchange(target, compare, exchange, old);
    }
};

#endif//ATOMIC_ASM_H_INCLUDED
