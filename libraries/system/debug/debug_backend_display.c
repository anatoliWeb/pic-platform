/*
 * File: libraries/system/debug/debug_backend_display.c
 *
 * DISPLAY backend for the debug facade. It owns the text cursor model
 * (row/column) and delegates low-level writes to the selected display
 * adapter. Only the display driver for the chosen transport is pulled in.
 */

#include "libraries/system/debug/debug_backend_display.h"
#include "libraries/system/debug/debug_display_lcd_2x16.h"

#if DRV_DEBUG_BACKEND_DISPLAY
#if (DRV_DEBUG_DISPLAY_TYPE_LCD_2X16 == 1)

#define DEBUG_DISPLAY_COLS 16u
#define DEBUG_DISPLAY_ROWS 2u

static uint8_t g_col = 0u;
static uint8_t g_row = 0u;

static void display_reset_cursor(void)
{
    g_row = 0u;
    g_col = 0u;
    debug_lcd_set_cursor(g_row, g_col);
}

void debug_display_init(void)
{
#if DRV_DEBUG_DISPLAY_AUTO_INIT
    debug_lcd_init();
    debug_lcd_clear();
    display_reset_cursor();
#endif
}

void debug_display_clear(void)
{
    debug_lcd_clear();
    display_reset_cursor();
}

void debug_display_newline(void)
{
    g_row = (uint8_t)((g_row + 1u) % DEBUG_DISPLAY_ROWS);
    g_col = 0u;
    debug_lcd_set_cursor(g_row, g_col);
}

void debug_display_write_char(char c)
{
    if (c == '\n')
    {
        debug_display_newline();
        return;
    }

    if (g_col >= DEBUG_DISPLAY_COLS)
    {
        debug_display_newline();
    }

    debug_lcd_set_cursor(g_row, g_col);
    debug_lcd_write_char(c);
    g_col++;
}

#endif /* DRV_DEBUG_DISPLAY_TYPE_LCD_2X16 */
#endif /* DRV_DEBUG_BACKEND_DISPLAY */
