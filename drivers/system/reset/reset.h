/*
 * File: drivers/system/reset/reset.h
 */

#ifndef DRIVERS_RESET_RESET_H
#define DRIVERS_RESET_RESET_H

#include "core/compiler.h"
#include "core/types.h"

typedef enum
{
    RESET_CAUSE_UNKNOWN = 0,
    RESET_CAUSE_POWER_ON,
    RESET_CAUSE_BROWN_OUT,
    RESET_CAUSE_WATCHDOG,
    RESET_CAUSE_EXTERNAL,
    RESET_CAUSE_SOFTWARE
} reset_cause_t;

void reset_init(void);
reset_cause_t reset_get_cause(void);
void reset_clear_flags(void);

#endif /* DRIVERS_RESET_RESET_H */
