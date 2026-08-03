/*
 * File: examples-projects/xc8/display/lcd_hd44780/i2c_pcf8574.X/main.c
 *
 * Direct demo of the PCF8574 I2C transport for the HD44780 controller
 * (libraries/display/lcd_hd44780/lcd_i2c.h). It calls the library directly,
 * without the debug facade.
 *
 * On init failure (backpack missing or miswired) the example never writes to
 * the LCD and idles in a fail-state loop. An optional status LED on RB0
 * blinks while the fail state is active.
 */

#include "project_config.h"

#include "core/delay.h"
#include "libraries/display/lcd_hd44780/lcd_i2c.h"

#define LCD_I2C_ADDR     0x27u
#define LCD_I2C_FREQ_HZ  100000u

#define FAIL_LED_TRIS   TRISBbits.TRISB0
#define FAIL_LED        LATBbits.LATB0

void main(void)
{
    lcd_i2c_status_t status = lcd_i2c_init(LCD_I2C_ADDR, LCD_I2C_FREQ_HZ);

    if (status != LCD_I2C_OK)
    {
        FAIL_LED_TRIS = 0u;
        FAIL_LED = 0u;

        while (1)
        {
            FAIL_LED ^= 1u;
            DRV_DELAY_MS(500u);
        }
    }

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
