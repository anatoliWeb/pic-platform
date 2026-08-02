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

#include "core/delay.h"

#if DRV_DEBUG_DISPLAY_INTERFACE_I2C
#include "drivers/communication/i2c/i2c.h"

/* PCF8574 to HD44780 backpack mapping. */
#define LCD_RS_BIT  0x01u
#define LCD_RW_BIT  0x02u
#define LCD_EN_BIT  0x04u
#define LCD_BL_BIT  0x08u

static void lcd_i2c_send(uint8_t data)
{
    i2c_start();
    (void)i2c_write_byte((uint8_t)((DRV_DEBUG_DISPLAY_I2C_ADDR) << 1u));
    (void)i2c_write_byte(data);
    i2c_stop();
}

static void lcd_i2c_pulse(uint8_t data)
{
    lcd_i2c_send((uint8_t)(data | LCD_EN_BIT));
    delay_us(2u);
    lcd_i2c_send((uint8_t)(data & (uint8_t)(~LCD_EN_BIT)));
    delay_us(2u);
}

static void lcd_i2c_nibble(uint8_t nibble, uint8_t rs)
{
    uint8_t out = LCD_BL_BIT;

    if (rs != 0u)
    {
        out |= LCD_RS_BIT;
    }

    out |= (uint8_t)((nibble & 0x0Fu) << 4u);
    lcd_i2c_pulse(out);
}

static void lcd_i2c_byte(uint8_t data, uint8_t rs)
{
    lcd_i2c_nibble((uint8_t)(data >> 4u), rs);
    lcd_i2c_nibble((uint8_t)(data & 0x0Fu), rs);

    if (data == 0x01u || data == 0x02u)
    {
        delay_ms(2u);
    }
    else
    {
        delay_us(40u);
    }
}

static void lcd_hd44780_init(void)
{
    delay_ms(20u);

    lcd_i2c_nibble(0x03u, 0u);
    delay_ms(5u);
    lcd_i2c_nibble(0x03u, 0u);
    delay_us(150u);
    lcd_i2c_nibble(0x03u, 0u);
    delay_us(150u);
    lcd_i2c_nibble(0x02u, 0u);
    delay_us(150u);

    lcd_i2c_byte(0x28u, 0u);
    lcd_i2c_byte(0x0Cu, 0u);
    lcd_i2c_byte(0x01u, 0u);
    lcd_i2c_byte(0x06u, 0u);
}

void debug_lcd_init(void)
{
#if DRV_DEBUG_DISPLAY_AUTO_INIT
    i2c_init((uint32_t)DRV_DEBUG_DISPLAY_I2C_FREQ);
#endif
    lcd_hd44780_init();
}

void debug_lcd_clear(void)
{
    lcd_i2c_byte(0x01u, 0u);
}

void debug_lcd_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t base = (row == 0u) ? 0x00u : 0x40u;
    lcd_i2c_byte((uint8_t)(0x80u | (uint8_t)(base + col)), 0u);
}

void debug_lcd_write_char(char c)
{
    lcd_i2c_byte((uint8_t)c, 1u);
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