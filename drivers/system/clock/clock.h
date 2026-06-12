#ifndef DRIVERS_CLOCK_CLOCK_H
#define DRIVERS_CLOCK_CLOCK_H

#include "core/compiler.h"
#include "core/device.h"

/*
 * Clock helper module.
 *
 * PIC18F452 oscillator mode is configured by project-level config bits.
 * This module exposes the configured oscillator frequency to drivers,
 * examples, timers, UART helpers and application code.
 */

#ifndef CLOCK_FREQUENCY
    #if defined(DRV_XTAL_FREQ)
        #define CLOCK_FREQUENCY ((uint32_t)DRV_XTAL_FREQ)
    #elif defined(PIC_PLATFORM_CLOCK_HZ)
        #define CLOCK_FREQUENCY ((uint32_t)PIC_PLATFORM_CLOCK_HZ)
    #elif defined(_XTAL_FREQ)
        #define CLOCK_FREQUENCY ((uint32_t)_XTAL_FREQ)
    #else
        #error "Clock frequency is not defined. Define DRV_XTAL_FREQ, PIC_PLATFORM_CLOCK_HZ or _XTAL_FREQ."
    #endif
#endif

#define CLOCK_INSTRUCTION_FREQUENCY ((uint32_t)(CLOCK_FREQUENCY / 4UL))

/*
 * Backward-compatible API.
 */
uint32_t clock_get_frequency(void);

/*
 * Explicit clock API.
 */
uint32_t clock_get_frequency_hz(void);
uint32_t clock_get_frequency_khz(void);
uint32_t clock_get_frequency_mhz(void);

uint32_t clock_get_instruction_frequency_hz(void);
uint32_t clock_get_instruction_frequency_khz(void);
uint32_t clock_get_instruction_frequency_mhz(void);

#endif /* DRIVERS_CLOCK_CLOCK_H */