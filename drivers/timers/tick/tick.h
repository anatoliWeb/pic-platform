/*
 * File: drivers/timers/tick/tick.h
 */

#ifndef DRIVERS_TICK_TICK_H
#define DRIVERS_TICK_TICK_H

#include "core/compiler.h"
#include "core/types.h"

void tick_init(void);
uint32_t tick_get(void);
void tick_delay(uint32_t ms);

#endif /* DRIVERS_TICK_TICK_H */
