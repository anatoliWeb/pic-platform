/*
 * File: C18/libraries/system/uart_debug/uart_debug.c
 */

#include "libraries/system/uart_debug/uart_debug.h"

#if defined(DRV_DEBUG_ENABLE) && (DRV_DEBUG_ENABLE == 1) && defined(DRV_USE_UART) && (DRV_USE_UART == 1)

static void dbg_write_unsigned(uint16_t value)
{
    char buffer[5];
    uint8_t i = 0u;

    if (value == 0u)
    {
        DBG_WRITE_BYTE('0');
        return;
    }

    while (value > 0u)
    {
        buffer[i++] = (char)('0' + (value % 10u));
        value /= 10u;
    }

    while (i > 0u)
    {
        DBG_WRITE_BYTE((uint8_t)buffer[--i]);
    }
}

void dbg_print_int(int value)
{
    if (value < 0)
    {
        DBG_WRITE_BYTE('-');
        dbg_write_unsigned((uint16_t)(-value));
    }
    else
    {
        dbg_write_unsigned((uint16_t)value);
    }
}

void dbg_print_hex(uint8_t value)
{
    const char hex_chars[] = "0123456789ABCDEF";

    DBG_WRITE_BYTE('0');
    DBG_WRITE_BYTE('x');
    DBG_WRITE_BYTE((uint8_t)hex_chars[(value >> 4) & 0x0Fu]);
    DBG_WRITE_BYTE((uint8_t)hex_chars[value & 0x0Fu]);
}

#endif
