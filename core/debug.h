/*
 * File: core/debug.h
 *
 * Public debug facade entry point. Forwards to the extensible debug module
 * in libraries/system/debug while keeping the original UART debug macro
 * aliases for backward compatibility.
 */

#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#include "libraries/system/debug/debug.h"
#include "libraries/system/uart_debug/uart_debug.h"

/* Backward-compatible aliases for previous debug macros. */
#define DRV_DEBUG_INIT()       debug_init()
#define DRV_DEBUG_LOG(msg)     debug_write_line(msg)
#define DRV_DEBUG_LOG_HEX(v)   debug_write_hex8(v)

#endif /* CORE_DEBUG_H */