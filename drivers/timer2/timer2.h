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
void timer2_set_callback(void (*cb)(void));

/* Call this from MCU ISR when TMR2 overflow flag is set. */
void timer2_irq_handler(void);

#endif /* DRIVERS_TIMER2_TIMER2_H */
