/*
 * File: core/device.h
 */

#ifndef CORE_DEVICE_H
#define CORE_DEVICE_H

/*
 * Project-level clock selection.
 *
 * The preferred source of truth is PIC_PLATFORM_CLOCK_HZ. If a project only
 * provides DRV_XTAL_FREQ or _XTAL_FREQ, keep that value and derive the other
 * clock macros from it. Legacy examples may still opt into the default clock
 * through an explicit PIC_PLATFORM_ALLOW_DEFAULT_CLOCK define in project code.
 */

#include "core/config/project_config_template.h"

#ifndef PIC_PLATFORM_CLOCK_HZ
    #if defined(DRV_XTAL_FREQ)
        #define PIC_PLATFORM_CLOCK_HZ DRV_XTAL_FREQ
    #elif defined(_XTAL_FREQ)
        #define PIC_PLATFORM_CLOCK_HZ _XTAL_FREQ
    #elif defined(PIC_PLATFORM_ALLOW_DEFAULT_CLOCK)
        #define PIC_PLATFORM_CLOCK_HZ 8000000UL
    #else
        #error "PIC_PLATFORM_CLOCK_HZ must be defined before including core/device.h"
    #endif
#endif

#ifndef DRV_XTAL_FREQ
#define DRV_XTAL_FREQ PIC_PLATFORM_CLOCK_HZ
#endif

#ifndef _XTAL_FREQ
#define _XTAL_FREQ PIC_PLATFORM_CLOCK_HZ
#endif

#endif /* CORE_DEVICE_H */
