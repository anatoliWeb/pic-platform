/*
 * File: scripts/tests/fixtures/lcd_i2c_harness_old_sig.c
 *
 * Negative compile fixture: calls the removed signature
 * lcd_i2c_controller_init(uint8_t i2c_addr). The current API takes no
 * arguments, so this fixture must fail to compile.
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

uint8_t i2c_write_byte(uint8_t data)
{
    (void)data;
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

#include "libraries/display/lcd_hd44780/lcd_i2c.c"

void use_removed_signature(void)
{
    (void)lcd_i2c_controller_init(0x27u);
}
