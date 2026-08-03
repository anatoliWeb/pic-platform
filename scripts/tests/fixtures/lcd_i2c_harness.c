/*
 * File: scripts/tests/fixtures/lcd_i2c_harness.c
 *
 * XC8 compile fixture for the HD44780 I2C transport. It provides mock i2c
 * and delay functions and includes the real library source, then
 * exercises the public API. Compiled with the XC8 toolchain for PIC18F452
 * (see test_lcd_hd44780_i2c.py). The harness is also used for -E preprocessing
 * to verify the compiled translation unit.
 */

#define PIC_PLATFORM_CLOCK_HZ 8000000UL

#include <xc.h>
#include <stdint.h>

#pragma config OSC = HS

void i2c_init(uint32_t clock_hz)
{
    (void)clock_hz;
}

void i2c_start(void)
{
}

void i2c_stop(void)
{
}

void i2c_restart(void)
{
}

uint8_t i2c_write_byte(uint8_t data)
{
    (void)data;
    return 0u;
}

uint8_t i2c_read_byte(uint8_t ack)
{
    (void)ack;
    return 0u;
}

void delay_us(uint16_t us)
{
    (void)us;
}

void delay_ms(uint16_t ms)
{
    (void)ms;
}

void micro_delay_us(uint16_t us)
{
    (void)us;
}

void micro_delay_cycles(uint16_t cycles)
{
    (void)cycles;
}

#include "libraries/display/lcd_hd44780/lcd_i2c.c"

void main(void)
{
    lcd_i2c_status_t s = lcd_i2c_init(0x27u, 100000u);

    if (s == LCD_I2C_OK)
    {
        lcd_i2c_clear();
        lcd_i2c_home();
        lcd_i2c_set_cursor(0u, 0u);
        lcd_i2c_write_char('A');
        lcd_i2c_write_string("OK");
        lcd_i2c_backlight(1u);
    }

    if (lcd_i2c_attach(0x27u) == LCD_I2C_OK)
    {
        (void)lcd_i2c_controller_init();
    }
    (void)lcd_i2c_probe(0x27u);
    (void)lcd_i2c_last_status();
    (void)lcd_i2c_is_ready();

    while (1)
    {
    }
}
