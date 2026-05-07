#ifndef DRIVERS_TIMER0_TIMER0_H
#define DRIVERS_TIMER0_TIMER0_H

#include "core/compiler.h"
#include "core/types.h"

void timer0_init(uint16_t prescaler);
void timer0_start(void);
void timer0_stop(void);
void timer0_set(uint16_t value);
uint16_t timer0_get(void);

void timer0_enable_interrupt(void);
void timer0_disable_interrupt(void);
void timer0_set_callback(void (*cb)(void));

/* Call this from MCU ISR when TMR0 overflow flag is set. */
void timer0_irq_handler(void);

#endif /* DRIVERS_TIMER0_TIMER0_H */
