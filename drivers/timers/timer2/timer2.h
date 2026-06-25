/*
 * File: drivers/timers/timer2/timer2.h
 */

#ifndef DRIVERS_TIMER2_TIMER2_H
#define DRIVERS_TIMER2_TIMER2_H

#include "core/compiler.h"
#include "core/types.h"

void timer2_init(uint16_t prescaler);
void timer2_start(void);
void timer2_stop(void);
void timer2_set(uint16_t value);
uint16_t timer2_get(void);

void timer2_enable_interrupt(void);
void timer2_disable_interrupt(void);
void timer2_clear_interrupt_flag(void);
void timer2_set_callback(void (*cb)(void));
void (*timer2_get_callback(void))(void);
void timer2_set_period(uint8_t value);
void timer2_set_postscaler(uint8_t value);

/* Call this from MCU ISR when TMR2 overflow flag is set. */
void timer2_irq_handler(void);

#endif /* DRIVERS_TIMER2_TIMER2_H */
