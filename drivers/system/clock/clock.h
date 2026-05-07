#ifndef DRIVERS_CLOCK_CLOCK_H
#define DRIVERS_CLOCK_CLOCK_H

#include "core/compiler.h"
#include "core/device.h"

#define CLOCK_FREQUENCY DRV_XTAL_FREQ

uint32_t clock_get_frequency(void);

#endif /* DRIVERS_CLOCK_CLOCK_H */
