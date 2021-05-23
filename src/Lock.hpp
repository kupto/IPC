/*
 * Inter Process Communication (IPC) Lock utility.
 *
 * Copyright (c) 2021, Kupto
 *
 * Instances of class Held*Lock keep particular resource locked for duration
 * of their lifetime. Instances of class *Lock implement the lock itself.
 *
 * This is free software; published under the MIT license.
 * See the LICENSE file.
 */

#include "arch.h"

#pragma once

namespace IPC {
  class HeldPreemptionLock {
    unsigned long _flags;

  public:
    HeldPreemptionLock()
      : _flags(local_irq_save())
    { }
    ~HeldPreemptionLock()
    { local_irq_restore(_flags); }

    HeldPreemptionLock(const HeldPreemptionLock &) = delete;
    HeldPreemptionLock& operator=(const HeldPreemptionLock &) = delete;

    HeldPreemptionLock(HeldPreemptionLock &&) = default;
    HeldPreemptionLock& operator=(HeldPreemptionLock &&) = default;
  };

# ifdef ARCH_UNIPROCESSOR
#   define DEFINE_SPINLOCK(x)
#   define ACQUIRE_SPINLOCK(x) HeldPreemptionLock()
    using HeldSpinLock = HeldPreemptionLock;
# else
#   define DEFINE_SPINLOCK(x) SpinLock x
#   define ACQUIRE_SPINLOCK(x) HeldSpinLock(x)
#   error "TODO: implement spinlocks"
  class SpinLock;
  class HeldSpinLock;
# endif
}
