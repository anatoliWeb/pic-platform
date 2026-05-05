#ifndef CORE_DELAY_H
#define CORE_DELAY_H

#include "core/compiler.h"
#include "core/device.h"

/*
 * Blocking delay API used by timing-sensitive drivers.
 * This layer is intentionally simple and does not use hardware timers.
 */
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);

/*
 * Backward-compatible wrappers used across existing drivers.
 */
#ifdef DRV_DELAY_US
    #undef DRV_DELAY_US
#endif

#ifdef DRV_DELAY_MS
    #undef DRV_DELAY_MS
#endif

#define DRV_DELAY_US(us) delay_us((uint16_t)(us))
#define DRV_DELAY_MS(ms) delay_ms((uint16_t)(ms))

#endif /* CORE_DELAY_H */
