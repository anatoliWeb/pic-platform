/*
 * File: libraries/system/debug/debug.c
 *
 * Debug facade. Formats values and dispatches each textual operation to
 * every enabled textual backend. PINS-only operations just delegate to the
 * pins backend. Only the backend/ adapter sources for enabled backends are
 * pulled into this translation unit.
 */

#include "libraries/system/debug/debug.h"

#if DRV_DEBUG_ENABLE

#include "libraries/system/debug/debug_backend_uart.h"
#include "libraries/system/debug/debug_backend_display.h"
#include "libraries/system/debug/debug_backend_pins.h"

#if DRV_DEBUG_BACKEND_UART
    #include "libraries/system/debug/debug_backend_uart.c"
#endif

#if DRV_DEBUG_BACKEND_DISPLAY
    #include "libraries/system/debug/debug_backend_display.c"
    #include "libraries/system/debug/debug_display_lcd_2x16.c"
#endif

#if DRV_DEBUG_BACKEND_PINS
    #include "libraries/system/debug/debug_backend_pins.c"
    #include "libraries/system/debug/debug_pins_gpio.c"
#endif

/* =========================================================
 * Textual dispatch
 * ========================================================= */

static void debug_emit_char(char c)
{
#if DRV_DEBUG_BACKEND_UART
    debug_uart_write_char(c);
#endif
#if DRV_DEBUG_BACKEND_DISPLAY
    debug_display_write_char(c);
#endif
}

static void debug_emit_buffer(const char* s)
{
    if (s == (const char*)0)
    {
        return;
    }

    while (*s != '\0')
    {
        debug_emit_char(*s);
        s++;
    }
}

static void debug_emit_newline(void)
{
#if DRV_DEBUG_BACKEND_UART
    debug_uart_newline();
#endif
#if DRV_DEBUG_BACKEND_DISPLAY
    debug_display_newline();
#endif
}

void debug_write(const char* s)
{
    debug_emit_buffer(s);
}

void debug_write_char(char c)
{
    debug_emit_char(c);
}

void debug_write_line(const char* s)
{
    debug_emit_buffer(s);
    debug_emit_newline();
}

/* ===========================================================
 * Numeric formatting
 * ========================================================= */

static void debug_write_unsigned(uint16_t value)
{
    char buffer[6];
    uint8_t i = 0u;

    if (value == 0u)
    {
        debug_emit_char('0');
        return;
    }

    while (value > 0u)
    {
        buffer[i++] = (char)('0' + (uint8_t)(value % 10u));
        value = (uint16_t)(value / 10u);
    }

    while (i > 0u)
    {
        debug_emit_char(buffer[--i]);
    }
}

void debug_write_u8(uint8_t v)
{
    debug_write_unsigned((uint16_t)v);
}

void debug_write_u16(uint16_t v)
{
    debug_write_unsigned(v);
}

void debug_write_i16(int16_t v)
{
    if (v < 0)
    {
        debug_emit_char('-');
        debug_write_unsigned((uint16_t)(-(int16_t)v));
    }
    else
    {
        debug_write_unsigned((uint16_t)v);
    }
}

static void debug_write_hex_digits(uint8_t hi, uint8_t lo)
{
    static const char hex_chars[] = "0123456789ABCDEF";

    debug_emit_char(hex_chars[hi]);
    debug_emit_char(hex_chars[lo]);
}

void debug_write_hex8(uint8_t v)
{
    debug_write_hex_digits((uint8_t)((v >> 4) & 0x0Fu), (uint8_t)(v & 0x0Fu));
}

void debug_write_hex16(uint16_t v)
{
    debug_write_hex_digits((uint8_t)((v >> 12) & 0x0Fu), (uint8_t)((v >> 8) & 0x0Fu));
    debug_write_hex_digits((uint8_t)((v >> 4) & 0x0Fu), (uint8_t)(v & 0x0Fu));
}

/* ===========================================================
 * Control
 * ========================================================= */

void debug_init(void)
{
#if DRV_DEBUG_BACKEND_UART
    debug_uart_init();
#endif
#if DRV_DEBUG_BACKEND_DISPLAY
    debug_display_init();
#endif
#if DRV_DEBUG_BACKEND_PINS
    debug_pins_init();
#endif
}

void debug_clear(void)
{
#if DRV_DEBUG_BACKEND_DISPLAY
    debug_display_clear();
#endif
}

void debug_flush(void)
{
    /* UART writes are blocking; displays write immediately. */
}

#if DRV_DEBUG_BACKEND_PINS

void debug_pin_pulse(uint8_t channel)
{
    debug_pins_pulse(channel);
}

void debug_pin_set(uint8_t channel, uint8_t state)
{
    debug_pins_set(channel, state);
}

void debug_pin_code(uint8_t value)
{
    debug_pins_code(value);
}

void debug_pin_error(uint8_t code)
{
    debug_pins_error(code);
}

#endif /* DRV_DEBUG_BACKEND_PINS */

#endif /* DRV_DEBUG_ENABLE */