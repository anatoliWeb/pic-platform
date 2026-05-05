#ifndef CORE_COMPILER_H
#define CORE_COMPILER_H

/*
 * Compiler abstraction layer for PIC universal drivers.
 * Put all C18/XC8 specific details here.
 */

#include <stdint.h>
#include <stdbool.h>

/* =========================================================
 * Compiler detection
 * ========================================================= */
#if defined(__18CXX)

    #define DRV_COMPILER_C18 1
    #include <p18f458.h> /* Adjust target header if required */

#elif defined(__XC8)

    #define DRV_COMPILER_XC8 1
    #include <xc.h>

#else

    #error "Unsupported compiler. Supported: MPLAB C18 and MPLAB XC8."

#endif

/* =========================================================
 * Common abstraction macros
 * ========================================================= */
#if defined(DRV_COMPILER_C18)
    #define DRV_INLINE static
    #define DRV_ROM rom
#elif defined(DRV_COMPILER_XC8)
    #define DRV_INLINE static inline
    #define DRV_ROM const
#endif

#define DRV_UNUSED(x) ((void)(x))

/* =========================================================
 * Optional delay wrapper
 * ========================================================= */
#if defined(DRV_COMPILER_XC8)
    #include <libpic30.h>
    #define DRV_DELAY_US(us) __delay_us(us)
    #define DRV_DELAY_MS(ms) __delay_ms(ms)
#else
    /* C18: project can override these macros with custom delay functions */
    #define DRV_DELAY_US(us) do { DRV_UNUSED(us); } while (0)
    #define DRV_DELAY_MS(ms) do { DRV_UNUSED(ms); } while (0)
#endif

#endif /* CORE_COMPILER_H */
