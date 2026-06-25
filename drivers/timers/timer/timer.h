/*
 * File: drivers/timers/timer/timer.h
 */

#ifndef DRIVERS_TIMER_TIMER_H
#define DRIVERS_TIMER_TIMER_H

#include "core/compiler.h"
#include "core/types.h"

/* Generic timer placeholder.
 * Use timer0/timer1/timer2/timer3 drivers for concrete hardware timers.
 */
void timer_placeholder(void);

#endif /* DRIVERS_TIMER_TIMER_H */
