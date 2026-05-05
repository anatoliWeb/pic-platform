#ifndef CORE_DEVICE_H
#define CORE_DEVICE_H

#include "core/compiler.h"

/*
 * Device-level defaults.
 * Override these values in project build settings or before including this file.
 */
#ifndef DRV_DEVICE_NAME
    #define DRV_DEVICE_NAME "PIC18F458"
#endif

/*
 * Single source of truth for MCU frequency (Hz).
 * All drivers should use DRV_XTAL_FREQ for timing-related calculations.
 */
#ifndef DRV_XTAL_FREQ
    #define DRV_XTAL_FREQ 8000000UL
#endif

/*
 * Backward compatibility macro used by many XC8/C18 delay utilities.
 */
#ifndef _XTAL_FREQ
    #define _XTAL_FREQ DRV_XTAL_FREQ
#endif

#endif /* CORE_DEVICE_H */
