/*
 * Arduino AVR IRQ primitives.
 *
 * Copyright (c) 2021, Kupto
 *
 * This file mirrors irqflags.h of Linux kernel.
 *
 * This is free software; published under the MIT license.
 * See the LICENSE file.
 */

#include <avr/interrupt.h>

#pragma once

static inline void arch_local_irq_disable(void)
{
	cli(); /* Implies MB */
}

static inline void arch_local_irq_enable(void)
{
	sei(); /* Implies MB */
}

static inline int arch_irqs_disabled(void)
{
	return bit_is_clear(SREG, SREG_I);
}

static inline int arch_irqs_disabled_flags(unsigned long flags)
{
	return !(flags & (1 << SREG_I));
}

static inline unsigned long arch_local_save_flags(void)
{
	return SREG;
}

static inline unsigned long arch_local_irq_save(void)
{
	unsigned long flags = arch_local_save_flags();

	arch_local_irq_disable();
	return flags;
}

static inline void arch_local_irq_restore(unsigned long flags)
{
	/* TODO: Why not set SREG with masked value, without branching? */
	if (arch_irqs_disabled_flags(flags))
		arch_local_irq_disable();
	else
		arch_local_irq_enable();
}
