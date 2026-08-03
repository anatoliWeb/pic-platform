/*
 * File: examples-projects/xc8/display/lcd_hd44780/i2c_pcf8574.X/main.c
 *
 * Direct demo of the PCF8574 I2C transport for the HD44780 controller
 * (libraries/display/lcd_hd44780/lcd_i2c.h). It calls the library directly,
 * without the debug facade.
 */

#include "project_config.h"

#include "core/delay.h"
#include "libraries/display/lcd_hd44780/lcd_i2c.h"

#define LCD_I2C_ADDR     0x27u
#define LCD_I2C_FREQ_HZ  100000u

void main(void)
{
    lcd_i2c_init(LCD_I2C_ADDR, LCD_I2C_FREQ_HZ);

    lcd_i2c_clear();
    lcd_i2c_set_cursor(0u, 0u);
    lcd_i2c_write_string("LCD I2C OK");
    lcd_i2c_set_cursor(1u, 0u);
    lcd_i2c_write_string("PCF8574");

    while (1)
    {
        DRV_DELAY_MS(1000u);
    }
}
