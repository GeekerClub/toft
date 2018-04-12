// for one byte types
template <>
struct AtomicAsmWidth<1> :
    public AtomicDefaultGet,
    public AtomicDefaultSet,
    public AtomicCompareExchangeEmulated<AtomicAsmWidth<1> >
{
    using AtomicDefaultSet::Set;

    template <typename T>
    static void Exchange(volatile T* target, T value, T* old)
    {
        __asm__ __volatile__(
            "xchgb %0, %1"
            :"=q"(*old)
            :"m"(*target), "0"(value)
            :"memory");
    }

    template <typename T>
    static T ExchangeAdd(volatile T* target, T value)
    {
        __asm__ __volatile__
        (
            "lock; xaddb %0, %1;"
            :"=q"(value)
            :"m"(*target), "0"(value)
        );
        return value;
    }

    template <typename T>
    static bool CompareExchange(volatile T* target, T compare, T exchange, T* old)
    {
        bool result;
        __asm__ __volatile__(
            "lock; cmpxchgb %2, %3\n"
            "setz %1"
            : "=a"(*old), "=q"(result)
            : "q"(exchange), "m"(*target), "0"(compare)
            : "memory"
        );
        return result;
    }
};

// for two bytes types
template <>
struct AtomicAsmWidth<2> :
    public AtomicDefaultGet,
    public AtomicDefaultSet,
    public AtomicCompareExchangeEmulated<AtomicAsmWidth<2> >
{
    using AtomicDefaultSet::Set;

    template <typename T>
    static void Exchange(volatile T* target, T value, T* old)
    {
        __asm__ __volatile__(
            "xchgw %0, %1"
            :"=r"(*old)
            :"m"(*target), "0"(value)
            :"memory");
    }

    template <typename T>
    static T ExchangeAdd(volatile T* target, T value)
    {
        __asm__ __volatile__
        (
            "lock; xaddw %0, %1;"
            :"=r"(value)
            :"m"(*target), "0"(value)
        );
        return value;
    }

    template <typename T>
    static bool CompareExchange(volatile T* target, T compare, T exchange, T* old)
    {
        bool result;
        __asm__ __volatile__(
            "lock; cmpxchgw %2, %3\n"
            "setz %1"
            : "=a"(*old), "=q"(result)
            : "r"(exchange), "m"(*target), "0"(compare)
            : "memory"
        );
        return result;
    }
};

// for four bytes types
template <>
struct AtomicAsmWidth<4> :
    public AtomicDefaultGet,
    public AtomicDefaultSet,
    public AtomicCompareExchangeEmulated<AtomicAsmWidth<4> >
{
    using AtomicDefaultSet::Set;

    template <typename T>
    static void Exchange(volatile T* target, T value, T* old)
    {
        __asm__ __volatile__(
            "xchgl %0, %1"
            :"=r"(*old)
            :"m"(*target), "0"(value)
            :"memory");
    }

    template <typename T>
    static T ExchangeAdd(volatile T* target, T value)
    {
        __asm__ __volatile__
        (
            "lock; xaddl %0, %1;"
            :"=r"(value)
            :"m"(*target), "0"(value)
        );
        return value;
    }

    template <typename T>
    static bool CompareExchange(volatile T* target, T compare, T exchange, T* old)
    {
        bool result;
        __asm__ __volatile__(
            "lock; cmpxchgl %2, %3\n\t"
            "setz %1"
            : "=a"(*old), "=q"(result)
            : "r"(exchange), "m"(*target), "0"(compare)
            : "memory"
        );
        return result;
    }
};

