#ifndef DRIVERS_TIMER3_TIMER3_H
#define DRIVERS_TIMER3_TIMER3_H

#include "core/compiler.h"
#include "core/types.h"

void timer3_init(uint16_t prescaler);
void timer3_start(void);
void timer3_stop(void);
void timer3_set(uint16_t value);
uint16_t timer3_get(void);

void timer3_enable_interrupt(void);
void timer3_disable_interrupt(void);
void timer3_set_callback(void (*cb)(void));

/* Call this from MCU ISR when TMR3 overflow flag is set. */
void timer3_irq_handler(void);

#endif /* DRIVERS_TIMER3_TIMER3_H */
