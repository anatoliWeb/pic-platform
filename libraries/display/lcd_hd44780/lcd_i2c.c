/*
 * File: libraries/display/lcd_hd44780/lcd_i2c.c
 */

#include "libraries/display/lcd_hd44780/lcd_i2c.h"

#include "core/delay.h"
#include "drivers/communication/i2c/i2c.h"

/*
 * PCF8574 to HD44780 backpack mapping.
 * P0=RS, P1=RW, P2=EN, P3=Backlight, P4=D4, P5=D5, P6=D6, P7=D7.
 */
#define LCD_I2C_PIN_RS   0x01u
#define LCD_I2C_PIN_RW   0x02u
#define LCD_I2C_PIN_EN   0x04u
#define LCD_I2C_PIN_BL   0x08u

#define LCD_I2C_ADDR_SHIFT 1u

static uint8_t g_i2c_addr = 0u;
static uint8_t g_backlight = LCD_I2C_PIN_BL;

static void lcd_i2c_send(uint8_t data)
{
    i2c_start();
    (void)i2c_write_byte((uint8_t)(g_i2c_addr << LCD_I2C_ADDR_SHIFT));
    (void)i2c_write_byte(data);
    i2c_stop();
}

static void lcd_i2c_pulse(uint8_t data)
{
    lcd_i2c_send((uint8_t)(data | LCD_I2C_PIN_EN));
    delay_us(2u);
    lcd_i2c_send((uint8_t)(data & (uint8_t)(~LCD_I2C_PIN_EN)));
    delay_us(2u);
}

static void lcd_i2c_nibble(uint8_t nibble, uint8_t rs)
{
    uint8_t out = g_backlight;

    if (rs != 0u)
    {
        out |= LCD_I2C_PIN_RS;
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

void lcd_i2c_init(uint8_t i2c_addr, uint32_t i2c_clock_hz)
{
    g_i2c_addr = i2c_addr;
    g_backlight = LCD_I2C_PIN_BL;
    i2c_init(i2c_clock_hz);
    lcd_hd44780_init();
}

void lcd_i2c_clear(void)
{
    lcd_i2c_byte(0x01u, 0u);
}

void lcd_i2c_home(void)
{
    lcd_i2c_byte(0x02u, 0u);
}

void lcd_i2c_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t base = (row == 0u) ? 0x00u : 0x40u;
    lcd_i2c_byte((uint8_t)(0x80u | (uint8_t)(base + col)), 0u);
}

void lcd_i2c_write_char(char c)
{
    lcd_i2c_byte((uint8_t)c, 1u);
}

void lcd_i2c_write_string(const char* str)
{
    if (str == (const char*)0)
    {
        return;
    }

    while (*str != '\0')
    {
        lcd_i2c_write_char(*str);
        str++;
    }
}

void lcd_i2c_backlight(uint8_t on)
{
    if (on != 0u)
    {
        g_backlight |= LCD_I2C_PIN_BL;
    }
    else
    {
        g_backlight &= (uint8_t)(~LCD_I2C_PIN_BL);
    }

    lcd_i2c_send(g_backlight);
}