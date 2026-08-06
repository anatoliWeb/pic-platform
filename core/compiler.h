/*
 * File: core/compiler.h
 */

#ifndef CORE_COMPILER_H
#define CORE_COMPILER_H

/*
 * Compiler abstraction layer for PIC universal drivers.
 * Put all C18/XC8 specific details here.
 */

/* =========================================================
 * Compiler detection
 * ========================================================= */

/* Built-in compiler macros have highest priority */
#if defined(__XC8)
    #define DRV_COMPILER_XC8 1

#elif defined(__18CXX)
    #define DRV_COMPILER_C18 1

/* Manual override (optional) */
#elif defined(DRV_COMPILER_XC8) && !defined(DRV_COMPILER_C18)
    /* user defined XC8 */

#elif defined(DRV_COMPILER_C18) && !defined(DRV_COMPILER_XC8)
    /* user defined C18 */

#else
    #error "Unsupported compiler. Define DRV_COMPILER_XC8 or DRV_COMPILER_C18"
#endif

/* Safety check */
#if defined(DRV_COMPILER_XC8) && defined(DRV_COMPILER_C18)
    #error "Both XC8 and C18 defined"
#endif

#if !defined(DRV_COMPILER_XC8) && !defined(DRV_COMPILER_C18)
    #error "No compiler defined"
#endif

/* =========================================================
 * Standard headers / types
 * ========================================================= */

#if defined(DRV_COMPILER_XC8)

    #include <xc.h>
    #include <stdint.h>
    #include <stdbool.h>

#elif defined(DRV_COMPILER_C18)

    /* C18 has no stdint/stdbool */

    typedef unsigned char      uint8_t;
    typedef unsigned int       uint16_t;
    typedef unsigned long      uint32_t;

    typedef signed char        int8_t;
    typedef signed int         int16_t;
    typedef signed long        int32_t;

    typedef uint8_t            bool;

    #ifndef true
        #define true  1
    #endif

    #ifndef false
        #define false 0
    #endif

#endif

/* =========================================================
 * Common macros
 * ========================================================= */

#if defined(DRV_COMPILER_C18)
    #define DRV_INLINE static
    #define DRV_ROM rom

#elif defined(DRV_COMPILER_XC8)
    #define DRV_INLINE static inline
    #define DRV_ROM const
#endif

/* =========================================================
 * Interrupt control
 *
 * DRV_INT_DISABLE / DRV_INT_ENABLE are simple toggle macros.
 * They are unsafe inside ISR context because DRV_INT_ENABLE
 * unconditionally re-enables interrupts regardless of the
 * entry state.
 *
 * For ISR-safe critical sections use the save/restore pair:
 *   drv_int_state_t saved;
 *   DRV_INT_SAVE_AND_DISABLE(saved);
 *   ... critical section ...
 *   DRV_INT_RESTORE(saved);
 *
 * These are safe whether called from main loop or ISR because
 * the restore writes back the exact GIE value that was present
 * before the disable.
 * ========================================================= */

typedef uint8_t drv_int_state_t;

#define DRV_INT_DISABLE()  do { INTCONbits.GIE = 0; } while (0)
#define DRV_INT_ENABLE()   do { INTCONbits.GIE = 1; } while (0)

#define DRV_INT_SAVE_AND_DISABLE(state) \
    do { (state) = INTCONbits.GIE; INTCONbits.GIE = 0; } while (0)

#define DRV_INT_RESTORE(state) \
    do { INTCONbits.GIE = (state); } while (0)

/* =========================================================
 * Utils
 * ========================================================= */

#define DRV_UNUSED(x) ((void)(x))

#endif /* CORE_COMPILER_H */
