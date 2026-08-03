/*
 * File: libraries/system/debug/debug_display_lcd_2x16.c
 *
 * Character LCD 2x16 adapter used by the DISPLAY backend. Two transports are
 * supported, selected by DRV_DEBUG_DISPLAY_INTERFACE_*:
 *
 *   I2C      - HD44780 behind a PCF8574 backpack, driven by the shared I2C
 *              master (address DRV_DEBUG_DISPLAY_I2C_ADDR).
 *   PARALLEL - HD44780 in direct 4-bit mode through libraries/display/
 *              lcd_hd44780. Because that driver needs its pin pointers set
 *              first, the project must call lcd_init_pins() before debug_init().
 *
 * Exactly one interface compiles. No display driver for the unselected
 * interface is referenced.
 */

#include "libraries/system/debug/debug_display_lcd_2x16.h"

#if DRV_DEBUG_BACKEND_DISPLAY && (DRV_DEBUG_DISPLAY_TYPE_LCD_2X16 == 1)

#if DRV_DEBUG_DISPLAY_INTERFACE_I2C
#include "libraries/display/lcd_hd44780/lcd_i2c.h"

void debug_lcd_init(void)
{
#if DRV_DEBUG_DISPLAY_AUTO_INIT
    lcd_i2c_init((uint8_t)DRV_DEBUG_DISPLAY_I2C_ADDR, (uint32_t)DRV_DEBUG_DISPLAY_I2C_FREQ);
#endif
}

void debug_lcd_clear(void)
{
    lcd_i2c_clear();
}

void debug_lcd_set_cursor(uint8_t row, uint8_t col)
{
    lcd_i2c_set_cursor(row, col);
}

void debug_lcd_write_char(char c)
{
    lcd_i2c_write_char(c);
}

#elif DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL

#include "libraries/display/lcd_hd44780/lcd.h"

void debug_lcd_init(void)
{
#if DRV_DEBUG_DISPLAY_AUTO_INIT
    lcd_init();
#endif
}

void debug_lcd_clear(void)
{
    lcd_clear();
}

void debug_lcd_set_cursor(uint8_t row, uint8_t col)
{
    lcd_set_cursor(row, col);
}

void debug_lcd_write_char(char c)
{
    lcd_write_char(c);
}

#endif /* DRV_DEBUG_DISPLAY_INTERFACE_* */

#endif /* DRV_DEBUG_BACKEND_DISPLAY && LCD_2X16 */
