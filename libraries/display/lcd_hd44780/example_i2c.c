/*
 * File: libraries/display/lcd_hd44780/example_i2c.c
 */

#include "core/compiler.h"
#include "libraries/display/lcd_hd44780/lcd_i2c.h"

#define LCD_I2C_ADDR 0x27u

void main(void)
{
    lcd_i2c_init(LCD_I2C_ADDR, 100000u);

    lcd_i2c_set_cursor(0u, 0u);
    lcd_i2c_write_string("Temp: 23.5C");

    lcd_i2c_set_cursor(1u, 0u);
    lcd_i2c_write_string("System OK");

    while (1)
    {
    }
}
