/*
 * File: libraries/system/debug/debug_backend_uart.h
 *
 * UART backend for the debug facade. Forwards textual debug output to the
 * shared UART driver. Compiled only when DRV_DEBUG_BACKEND_UART is 1.
 * Internal module header; do not include directly from application code.
 */

#ifndef LIBRARIES_SYSTEM_DEBUG_DEBUG_BACKEND_UART_H
#define LIBRARIES_SYSTEM_DEBUG_DEBUG_BACKEND_UART_H

#include "core/compiler.h"
#include "core/types.h"
#include "core/config.h"

#if DRV_DEBUG_BACKEND_UART

void debug_uart_init(void);
void debug_uart_write_char(char c);
void debug_uart_newline(void);

#endif /* DRV_DEBUG_BACKEND_UART */

#endif /* LIBRARIES_SYSTEM_DEBUG_DEBUG_BACKEND_UART_H */