#ifndef ATOMIC_GCC_H_INCLUDED
#define ATOMIC_GCC_H_INCLUDED

#if __i386__
#include "toft/system/atomic/atomic_asm/AtomicGccX86.h"
#elif __x86_64__
#include "toft/system/atomic/atomic_asm/AtomicGccX64.h"
#else
#error unsupported architect
#endif

#endif//ATOMIC_GCC_H_INCLUDED

