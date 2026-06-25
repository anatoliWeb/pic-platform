/*
 * File: C18/drivers/system/clock/clock.c
 */

#include "drivers/system/clock/clock.h"

uint32_t clock_get_frequency(void)
{
    return DRV_XTAL_FREQ;
}
