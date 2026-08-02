/*
 * File: libraries/system/debug/debug_backend_uart.c
 *
 * UART backend for the debug facade. This translation unit must be built
 * from the enclosing debug module (included by debug.c or listed in the
 * project) only when DRV_DEBUG_BACKEND_UART is 1.
 */

#include "libraries/system/debug/debug_backend_uart.h"
#include "drivers/communication/uart/uart.h"

#if DRV_DEBUG_BACKEND_UART
#if !defined(DRV_DEBUG_UART_BAUD)
    #define DRV_DEBUG_UART_BAUD 9600UL
#endif

void debug_uart_init(void)
{
    uart_init((uint32_t)DRV_DEBUG_UART_BAUD);
}

void debug_uart_write_char(char c)
{
    uart_write_byte((uint8_t)c);
}

void debug_uart_newline(void)
{
    uart_write_byte((uint8_t)'\r');
    uart_write_byte((uint8_t)'\n');
}

#endif /* DRV_DEBUG_BACKEND_UART */