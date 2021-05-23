/*
 * Inter Process Communication (IPC) Signal utility.
 *
 * Copyright (c) 2021, Kupto
 *
 * Allows to notify of an occured event through setting a flag. Two kinds of
 * Signal exist. Each may be raised, and queried. Regular class Signal shows
 * the change in state immediately through the query() member, which returns
 * the value true, iff the signal was raised. Querying the signal resets it,
 * so any subsequent call to query() will return false, until it is raise()d
 * again.
 *
 * UpdatedSignal class doesn't report being raised immediately. Its update()
 * member method must be called first (possibly from loop()). After updating,
 * its query() member will return the same value, until updated again.  This
 * is useful for maintaining a consistent execution state (throughout single
 * iteration of main loop or a block of code).
 *
 * Note that the raise() count is not being kept. This means that the number
 * of signals received can't be used to implement counters, etc.
 *
 * This is free software; published under the MIT license.
 * See the LICENSE file.
 */

#include "Lock.hpp"

#pragma once

namespace IPC {

  class Signal {
  protected:
    volatile bool _pending; // TODO: atomic
    DEFINE_SPINLOCK(_lock);

  public:
    Signal()
      : _pending(false)
    { }

    void begin()
    { _pending = false; }
    void raise()
    { _pending = true; }

    bool query() { // True iff signaled (once)
      if (!_pending)
        return false;
      _pending = false; // No need to LOCK
      return true;
    }

    HeldSpinLock lock()
    { return ACQUIRE_SPINLOCK(_lock); }
  };

  class UpdatedSignal : public Signal {
  protected:
    volatile bool _risen;

  public:
    UpdatedSignal()
      : _risen(false)
    { }

    void begin()
    { _risen = _pending = false; }
    void update() {
      HeldSpinLock l = lock();

      _risen = _pending;
      _pending = false;
    }

    bool query() // True iff signaled (until updated)
    { return _risen; }
  };
}

