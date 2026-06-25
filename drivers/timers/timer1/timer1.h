/*
 * File: drivers/timers/timer1/timer1.h
 */

#ifndef DRIVERS_TIMER1_TIMER1_H
#define DRIVERS_TIMER1_TIMER1_H

#include "core/compiler.h"
#include "core/types.h"

void timer1_init(uint16_t prescaler);
void timer1_start(void);
void timer1_stop(void);
void timer1_set(uint16_t value);
uint16_t timer1_get(void);

void timer1_enable_interrupt(void);
void timer1_disable_interrupt(void);
void timer1_set_callback(void (*cb)(void));
void (*timer1_get_callback(void))(void);

/* Call this from MCU ISR when TMR1 overflow flag is set. */
void timer1_irq_handler(void);

#endif /* DRIVERS_TIMER1_TIMER1_H */
