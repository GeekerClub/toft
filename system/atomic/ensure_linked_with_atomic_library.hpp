// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: An Qin (anqin.qin@gmail.com)

#ifndef TOFT_SYSTEM_ATOMIC_ENSURE_LINKED_WITH_ATOMIC_LIBRARY_HPP
#define TOFT_SYSTEM_ATOMIC_ENSURE_LINKED_WITH_ATOMIC_LIBRARY_HPP
#pragma once

namespace toft {

#ifdef NDEBUG
inline void EnsureLinkedWithAtomicLibrary() {}
#else
void EnsureLinkedWithAtomicLibrary();
#endif

}

#endif // end defined TOFT_SYSTEM_ATOMIC_ENSURE_LINKED_WITH_ATOMIC_LIBRARY_HPP
