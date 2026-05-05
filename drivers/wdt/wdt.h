#ifndef DRIVERS_WDT_WDT_H
#define DRIVERS_WDT_WDT_H

#include "core/compiler.h"

/*
 * WDT is typically controlled by config bits.
 * Runtime enable/disable may be unavailable on many PIC18 MCUs.
 */

void wdt_clear(void);

#define WDT_CLEAR() wdt_clear()

#endif /* DRIVERS_WDT_WDT_H */
