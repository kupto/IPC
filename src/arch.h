/*
 * Inter Process Communication (IPC) Architecture specific definitions.
 *
 * Copyright (c) 2021, Kupto
 *
 * This is free software; published under the MIT license.
 * See the LICENSE file.
 */

#pragma once

#if defined(ARDUINO_ARCH_AVR)
#  include "arch\avr\irqs.h"
#  define ARCH_UNIPROCESSOR
#else
#  error "Current Platform is not supported."
#endif

#define local_irq_save arch_local_irq_save
#define local_irq_restore arch_local_irq_restore
