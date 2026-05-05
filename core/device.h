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
 * System clock in Hz.
 * Update this value to match your hardware clock configuration.
 */
#ifndef _XTAL_FREQ
    #define _XTAL_FREQ 8000000UL
#endif

#endif /* CORE_DEVICE_H */
