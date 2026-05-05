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

#endif /* CORE_COMPILER_H */
