# Arduino Inter Process Communication (IPC) Library
Very simple and easy to use, see examples. This is an Arduino
library, but the implementation is platform independent. Used
to communicate events between distinct parts of code.
TODO: Add examples

This is still in beta, changes and/or comments are welcome.

## Signals
Set a flag for other part of code to read. See the Example:

```C++
#include <Signal.hpp>
IPC::Signal s;

void signal_writer()
{
  s.raise();
}

void signal_reader()
{
  if (s.query())
    do_one_time_response();
}
```

can be used in all contexts, but does not guarantee any memory
ordering.  Writer asserts that signal has occured,  and reader
acknowledges by clearing the flag. Parallel calls to `raise()`
will overwrite each other, while reads in `query()` clear the
flag every time it might have occured. The number of calls to
`do_one_time_response()` will not match calls to `raise()`.

See `Event`s for that functionality.

### Member `update()`
Above is a fast form of 1:1 communication. Follows the Update
mechanism, which provides a synchronization of single signal,
with parallel invocations of `query()`.

Most IPC classes have two forms: updated and non-updated.  In
the later, the effect can be seen immediately. The prior form
implements a two-step event propagation. First, the change is
recorded (message sent or signal raised), but not yet visible
on the recipient side.  Only after the `update()` member gets
called, the change may be queried by the receiver.

The usage pattern for `IPC::UpdatedSignal::update()` follows:

```C++
#include <Signal.hpp>
IPC::UpdatedSignal us;

void signal_writer()
{
  us.raise(); // Same as s.raise()
}

void loop()
{
  s.update(); // Check for signal once per loop()

  if (s.query())
    do_one_time_response();
  // ...
  if (s.query())
    do_second_response();
}
```

Here the signal is read once and asserted for the duration of
the loop();


The rationale behind this is so that no asynchronous change
can change the internal state of currently executed section
of code.   The usage pattern is to first `update()` all IPC
instances, and then to query for events. Doing it this way,
new events can't appear from nowhere, making the logic more
concise.

## Locks
Locks are useful to prevent races and to protect resources.

### PreemptionLock
Class `HeldPreemptionLock`, disables interrupts for the duration
of its lifetime.

For example, given a global variable `int j`, accessed both from
an interrupt context:

```C++
  void irq_handler() {
    j++; // j will increment by 1
  }
```

and from a process context:

```C++
  j++; // This code can be preempted (no guarantee what j may become)
       // We must use a preemption lock:
  {    // Curly braces serve to highlight the scope
    HeldPreemptionLock l = HeldPreemptionLock();
    j++; // Code with IRQs disabled, j will increment by 1
  } // Here l goes out of scope, PreemptionLock is released
  ...
```
Note that on a uniprocessor system, PreemptionLock is sufficient
to achieve complete serialization.

# License
Copyright (c) 2021 Kupto - MIT License
