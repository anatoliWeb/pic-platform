/*
 * File: libraries/system/uart_debug/uart_debug.h
 */

#ifndef DRIVERS_UART_DEBUG_UART_DEBUG_H
#define DRIVERS_UART_DEBUG_UART_DEBUG_H

#include "core/compiler.h"
#include "core/config.h"
#include "drivers/communication/uart/uart.h"

#if defined(DRV_DEBUG_ENABLE) && (DRV_DEBUG_ENABLE == 1) && defined(DRV_USE_UART) && (DRV_USE_UART == 1)

void dbg_print_int(int value);
void dbg_print_hex(uint8_t value);

#define DBG_PRINT(str)          do { uart_write_string((str)); } while (0)
#define DBG_PRINTLN(str)        do { uart_write_string((str)); uart_write_string("\r\n"); } while (0)
#define DBG_WRITE_BYTE(b)       do { uart_write_byte((uint8_t)(b)); } while (0)

#define DBG_PRINT_INT(value)    do { dbg_print_int((value)); } while (0)
#define DBG_PRINT_HEX(value)    do { dbg_print_hex((uint8_t)(value)); } while (0)

#else

#define DBG_PRINT(str)          do { } while (0)
#define DBG_PRINTLN(str)        do { } while (0)
#define DBG_WRITE_BYTE(b)       do { } while (0)

#define DBG_PRINT_INT(value)    do { } while (0)
#define DBG_PRINT_HEX(value)    do { } while (0)

#endif

#endif /* DRIVERS_UART_DEBUG_UART_DEBUG_H */
