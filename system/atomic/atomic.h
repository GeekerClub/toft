// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_ATOMIC_ATOMIC_H
#define TOFT_SYSTEM_ATOMIC_ATOMIC_H
#pragma once

#include <features.h>

#if __GNUC_PREREQ(7, 0)
#elif __GNUC_PREREQ(4, 1)
#define TOFT_HAS_GCC_BUILDIN_ATOMIC_INTRINSICS
#endif

#if defined TOFT_HAS_GCC_BUILDIN_ATOMIC_INTRINSICS
#include "toft/system/atomic/functions.h"
#else
#include "toft/system/atomic/atomic_asm/atomic.h"
#endif

#undef TOFT_HAS_GCC_BUILDIN_ATOMIC_INTRINSICS


#include "toft/system/atomic/type.h"

#endif // TOFT_SYSTEM_ATOMIC_ATOMIC_H
