/*
 * File: libraries/system/debug/debug_display_lcd_2x16.h
 *
 * Character LCD 2x16 display adapter used by the DISPLAY backend.
 * Supports the I2C (standard PCF8574 backpack) and direct parallel
 * (4-bit, via libraries/display/lcd_hd44780) transports, chosen by
 * DRV_DEBUG_DISPLAY_INTERFACE_*. Internal adapter.
 */

#ifndef LIBRARIES_SYSTEM_DEBUG_DEBUG_DISPLAY_LCD_2X16_H
#define LIBRARIES_SYSTEM_DEBUG_DEBUG_DISPLAY_LCD_2X16_H

#include "core/compiler.h"
#include "core/types.h"
#include "core/config.h"

#if DRV_DEBUG_BACKEND_DISPLAY && (DRV_DEBUG_DISPLAY_TYPE_LCD_2X16 == 1)

void debug_lcd_init(void);
uint8_t debug_lcd_is_ready(void);
void debug_lcd_clear(void);
void debug_lcd_set_cursor(uint8_t row, uint8_t col);
void debug_lcd_write_char(char c);

#endif /* DRV_DEBUG_BACKEND_DISPLAY && LCD_2X16 */

#endif /* LIBRARIES_SYSTEM_DEBUG_DEBUG_DISPLAY_LCD_2X16_H */