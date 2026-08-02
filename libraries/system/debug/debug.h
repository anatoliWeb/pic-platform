/*
 * File: libraries/system/debug/debug.h
 *
 * Extensible compile-time debug facade for PIC18F452.
 *
 * The facade routes diagnostic output to one or more backends selected at
 * compile time through configuration defines. Backends are:
 *
 *   UART     - textual output over the shared UART driver
 *   DISPLAY  - textual output on up to four-line character display
 *   PINS     - non-textual diagnostic signals on GPIO outputs
 *
 * The facade never knows the transport details. Each backend is implemented
 * in its own file inside the module and compiled only when enabled, so the
 * firmware contains exactly the sources the project selected.
 *
 * When DRV_DEBUG_ENABLE is 0 the whole module compiles to no-ops and no debug
 * dependency is pulled into the image.
 */

#ifndef LIBRARIES_SYSTEM_DEBUG_DEBUG_H
#define LIBRARIES_SYSTEM_DEBUG_DEBUG_H

#include "core/compiler.h"
#include "core/config.h"
#include "core/types.h"

/* =========================================================
 * Global enable / disable
 * =========================================================
 * DRV_DEBUG_ENABLE is the single master switch (defined in core/config.h,
 * default 1). When it is 0 the debug facade, all backends and all debug
 * dependencies are compiled out.
 */

/* =========================================================
 * Backend selection (default: UART only)
 * ========================================================= */

#ifndef DRV_DEBUG_BACKEND_UART
    #define DRV_DEBUG_BACKEND_UART     1
#endif

#ifndef DRV_DEBUG_BACKEND_DISPLAY
    #define DRV_DEBUG_BACKEND_DISPLAY  0
#endif

#ifndef DRV_DEBUG_BACKEND_PINS
    #define DRV_DEBUG_BACKEND_PINS     0
#endif

/* =========================================================
 * UART backend configuration
 * =========================================================
 * Default baud matches the current UART drivers/examples (9600). The single
 * EUSART of PIC18F452 is always used, so no port instance is needed.
 */
#ifndef DRV_DEBUG_UART_BAUD
    #define DRV_DEBUG_UART_BAUD 9600UL
#endif

/* =========================================================
 * DISPLAY backend configuration
 * =========================================================
 * Select the concrete display type and exactly one transport.
 */
#ifndef DRV_DEBUG_DISPLAY_TYPE_LCD_2X16
    #define DRV_DEBUG_DISPLAY_TYPE_LCD_2X16 0
#endif

#ifndef DRV_DEBUG_DISPLAY_INTERFACE_I2C
    #define DRV_DEBUG_DISPLAY_INTERFACE_I2C 0
#endif

#ifndef DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL
    #define DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL 0
#endif

#ifndef DRV_DEBUG_DISPLAY_AUTO_INIT
    #define DRV_DEBUG_DISPLAY_AUTO_INIT 1
#endif

#ifndef DRV_DEBUG_DISPLAY_I2C_FREQ
    #define DRV_DEBUG_DISPLAY_I2C_FREQ 100000UL
#endif

/* PINS backend configuration. Only direct GPIO is currently supported. */
#ifndef DRV_DEBUG_PINS_INTERFACE_GPIO
    #define DRV_DEBUG_PINS_INTERFACE_GPIO 0
#endif

#ifndef DRV_DEBUG_PINS_INTERFACE_I2C
    #define DRV_DEBUG_PINS_INTERFACE_I2C 0
#endif

#ifndef DRV_DEBUG_PINS_CHANNEL_COUNT
    #define DRV_DEBUG_PINS_CHANNEL_COUNT 4
#endif

#ifndef DRV_DEBUG_PINS_PULSE_US
    #define DRV_DEBUG_PINS_PULSE_US 10000UL
#endif

#if (DRV_DEBUG_ENABLE != 0) && (DRV_DEBUG_ENABLE != 1)
    #error "DRV_DEBUG_ENABLE must be 0 or 1"
#endif

#if (DRV_DEBUG_BACKEND_UART != 0) && (DRV_DEBUG_BACKEND_UART != 1)
    #error "DRV_DEBUG_BACKEND_UART must be 0 or 1"
#endif

#if (DRV_DEBUG_BACKEND_DISPLAY != 0) && (DRV_DEBUG_BACKEND_DISPLAY != 1)
    #error "DRV_DEBUG_BACKEND_DISPLAY must be 0 or 1"
#endif

#if (DRV_DEBUG_BACKEND_PINS != 0) && (DRV_DEBUG_BACKEND_PINS != 1)
    #error "DRV_DEBUG_BACKEND_PINS must be 0 or 1"
#endif

#if (DRV_DEBUG_DISPLAY_TYPE_LCD_2X16 != 0) && (DRV_DEBUG_DISPLAY_TYPE_LCD_2X16 != 1)
    #error "DRV_DEBUG_DISPLAY_TYPE_LCD_2X16 must be 0 or 1"
#endif

#if (DRV_DEBUG_DISPLAY_INTERFACE_I2C != 0) && (DRV_DEBUG_DISPLAY_INTERFACE_I2C != 1)
    #error "DRV_DEBUG_DISPLAY_INTERFACE_I2C must be 0 or 1"
#endif

#if (DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL != 0) && (DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL != 1)
    #error "DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL must be 0 or 1"
#endif

#if (DRV_DEBUG_DISPLAY_AUTO_INIT != 0) && (DRV_DEBUG_DISPLAY_AUTO_INIT != 1)
    #error "DRV_DEBUG_DISPLAY_AUTO_INIT must be 0 or 1"
#endif

#if (DRV_DEBUG_PINS_INTERFACE_GPIO != 0) && (DRV_DEBUG_PINS_INTERFACE_GPIO != 1)
    #error "DRV_DEBUG_PINS_INTERFACE_GPIO must be 0 or 1"
#endif

#if (DRV_DEBUG_PINS_INTERFACE_I2C != 0) && (DRV_DEBUG_PINS_INTERFACE_I2C != 1)
    #error "DRV_DEBUG_PINS_INTERFACE_I2C must be 0 or 1"
#endif

/* =========================================================
 * Severity filtering
 * =========================================================
 * Higher level enables more verbose calls. The level filter does not add
 * runtime cost: debug_error/debug_warn/... compile out below the threshold.
 */
#define DRV_DEBUG_LEVEL_ERROR  0
#define DRV_DEBUG_LEVEL_WARN   1
#define DRV_DEBUG_LEVEL_INFO   2
#define DRV_DEBUG_LEVEL_TRACE  3

#ifndef DRV_DEBUG_LEVEL
    #define DRV_DEBUG_LEVEL DRV_DEBUG_LEVEL_INFO
#endif

#if (DRV_DEBUG_LEVEL < DRV_DEBUG_LEVEL_ERROR) || (DRV_DEBUG_LEVEL > DRV_DEBUG_LEVEL_TRACE)
    #error "DRV_DEBUG_LEVEL must be one of DRV_DEBUG_LEVEL_ERROR/WARN/INFO/TRACE"
#endif

/* =========================================================
 * Compile-time validation (only when debug is enabled)
 * =========================================================
 */
#if DRV_DEBUG_ENABLE

    #if (DRV_DEBUG_BACKEND_UART + DRV_DEBUG_BACKEND_DISPLAY + DRV_DEBUG_BACKEND_PINS) == 0
        #error "DRV_DEBUG_BACKEND_* all zero: enable at least one debug backend"
    #endif

    #if DRV_DEBUG_BACKEND_DISPLAY

        /* Up to one LCD display type must be selected. */
        #if (DRV_DEBUG_DISPLAY_TYPE_LCD_2X16 != 1)
            #error "DRV_DEBUG_BACKEND_DISPLAY requires a display type (set DRV_DEBUG_DISPLAY_TYPE_LCD_2X16 to 1)"
        #endif

        /* Exactly one display transport must be selected. */
        #if (DRV_DEBUG_DISPLAY_INTERFACE_I2C + DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL) != 1
            #error "Select exactly one debug display interface (DRV_DEBUG_DISPLAY_INTERFACE_I2C or _PARALLEL)"
        #endif

        #if DRV_DEBUG_DISPLAY_INTERFACE_I2C && !defined(DRV_DEBUG_DISPLAY_I2C_ADDR)
            #error "Debug display I2C address is not configured (define DRV_DEBUG_DISPLAY_I2C_ADDR)"
        #endif
    #endif

    #if DRV_DEBUG_BACKEND_PINS
        /* Exactly one pins transport must be selected. */
        #if (DRV_DEBUG_PINS_INTERFACE_GPIO + DRV_DEBUG_PINS_INTERFACE_I2C) != 1
            #error "DRV_DEBUG_BACKEND_PINS requires a pins interface (set DRV_DEBUG_PINS_INTERFACE_GPIO to 1)"
        #endif

        #if DRV_DEBUG_PINS_INTERFACE_I2C
            #error "Debug pins I2C transport is not supported: no I2C expander library in pic-platform"
        #endif

        #if DRV_DEBUG_PINS_INTERFACE_GPIO
            #if (DRV_DEBUG_PINS_CHANNEL_COUNT < 1) || (DRV_DEBUG_PINS_CHANNEL_COUNT > 8)
                #error "DRV_DEBUG_PINS_CHANNEL_COUNT must be between 1 and 8"
            #endif
            #if (DRV_DEBUG_PINS_START_BIT > 7u)
                #error "DRV_DEBUG_PINS_START_BIT must be between 0 and 7"
            #endif
            #if (DRV_DEBUG_PINS_START_BIT + DRV_DEBUG_PINS_CHANNEL_COUNT) > 8u
                #error "DRV_DEBUG_PINS_START_BIT + DRV_DEBUG_PINS_CHANNEL_COUNT must not exceed 8"
            #endif
            #if (DRV_DEBUG_PINS_PULSE_US < 0) || (DRV_DEBUG_PINS_PULSE_US > 65535UL)
                #error "DRV_DEBUG_PINS_PULSE_US must fit in uint16_t"
            #endif
            #if !defined(DRV_DEBUG_PINS_PORT) || !defined(DRV_DEBUG_PINS_TRIS)
                #error "Direct GPIO debug pins require DRV_DEBUG_PINS_PORT and DRV_DEBUG_PINS_TRIS registers"
            #endif
            #ifndef DRV_DEBUG_PINS_START_BIT
                #error "Direct GPIO debug pins require DRV_DEBUG_PINS_START_BIT (first channel bit)"
            #endif
        #endif
    #endif

#endif /* DRV_DEBUG_ENABLE */

#if DRV_DEBUG_ENABLE

/* =========================================================
 * Public API
 * =========================================================
 */

void debug_init(void);
void debug_clear(void);
void debug_write(const char* s);
void debug_write_line(const char* s);
void debug_write_char(char c);
void debug_write_u8(uint8_t v);
void debug_write_u16(uint16_t v);
void debug_write_i16(int16_t v);
void debug_write_hex8(uint8_t v);
void debug_write_hex16(uint16_t v);
void debug_flush(void);

/* Severity-gated helpers. Every level compiles out below the threshold. */
#if (DRV_DEBUG_LEVEL >= DRV_DEBUG_LEVEL_ERROR)
    #define debug_error(s)  debug_write_line(s)
#else
    #define debug_error(s)  do { } while (0)
#endif

#if (DRV_DEBUG_LEVEL >= DRV_DEBUG_LEVEL_WARN)
    #define debug_warn(s)   debug_write_line(s)
#else
    #define debug_warn(s)   do { } while (0)
#endif

#if (DRV_DEBUG_LEVEL >= DRV_DEBUG_LEVEL_INFO)
    #define debug_info(s)   debug_write_line(s)
#else
    #define debug_info(s)   do { } while (0)
#endif

#if (DRV_DEBUG_LEVEL >= DRV_DEBUG_LEVEL_TRACE)
    #define debug_trace(s)  debug_write_line(s)
#else
    #define debug_trace(s)  do { } while (0)
#endif

/* PINS backend signals. */
#if DRV_DEBUG_BACKEND_PINS
    void debug_pin_pulse(uint8_t channel);
    void debug_pin_set(uint8_t channel, uint8_t state);
    void debug_pin_code(uint8_t value);
    void debug_pin_error(uint8_t code);
#else
    #define debug_pin_pulse(ch)   do { } while (0)
    #define debug_pin_set(ch, st) do { } while (0)
    #define debug_pin_code(v)     do { } while (0)
    #define debug_pin_error(c)    do { } while (0)
#endif

#else /* DRV_DEBUG_ENABLE == 0: everything degenerates to no-ops. */

#define debug_init()                 do { } while (0)
#define debug_clear()                do { } while (0)
#define debug_write(s)               do { } while (0)
#define debug_write_line(s)          do { } while (0)
#define debug_write_char(c)          do { } while (0)
#define debug_write_u8(v)            do { } while (0)
#define debug_write_u16(v)           do { } while (0)
#define debug_write_i16(v)           do { } while (0)
#define debug_write_hex8(v)          do { } while (0)
#define debug_write_hex16(v)         do { } while (0)
#define debug_flush()                do { } while (0)

#define debug_error(s)               do { } while (0)
#define debug_warn(s)                do { } while (0)
#define debug_info(s)                do { } while (0)
#define debug_trace(s)               do { } while (0)

#define debug_pin_pulse(ch)          do { } while (0)
#define debug_pin_set(ch, st)        do { } while (0)
#define debug_pin_code(v)            do { } while (0)
#define debug_pin_error(c)           do { } while (0)

#endif /* DRV_DEBUG_ENABLE */

#endif /* LIBRARIES_SYSTEM_DEBUG_DEBUG_H */
