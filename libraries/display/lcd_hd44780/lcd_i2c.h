/*
 * File: libraries/display/lcd_hd44780/lcd_i2c.h
 *
 * Reusable PCF8574 I2C transport for HD44780 character LCDs.
 *
 * PCF8574 to HD44780 backpack mapping (common):
 *   P0 = RS,  P1 = RW,  P2 = EN,  P3 = Backlight
 *   P4 = D4,  P5 = D5,  P6 = D6,  P7 = D7
 *
 * The transport owns the I2C transaction, EN pulse, and HD44780
 * init sequence. Callers use only the public API below.
 *
 * No malloc, no framebuffer. Supports 2-row HD44780 controllers
 * (row addresses 0x00/0x40); 2-line displays are the common target.
 */

#ifndef DRIVERS_LCD_HD44780_LCD_I2C_H
#define DRIVERS_LCD_HD44780_LCD_I2C_H

#include "core/compiler.h"
#include "core/types.h"

void lcd_i2c_init(uint8_t i2c_addr, uint32_t i2c_clock_hz);
void lcd_i2c_clear(void);
void lcd_i2c_home(void);
void lcd_i2c_set_cursor(uint8_t row, uint8_t col);
void lcd_i2c_write_char(char c);
void lcd_i2c_write_string(const char* str);
void lcd_i2c_backlight(uint8_t on);

#endif /* DRIVERS_LCD_HD44780_LCD_I2C_H */