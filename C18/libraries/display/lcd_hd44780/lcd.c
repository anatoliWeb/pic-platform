/*
 * File: C18/libraries/display/lcd_hd44780/lcd.c
 */

#include "libraries/display/lcd_hd44780/lcd.h"

#include "core/bit_utils.h"
#include "core/delay.h"

static volatile uint8_t* g_data_port = (volatile uint8_t*)0;
static volatile uint8_t* g_data_tris = (volatile uint8_t*)0;
static volatile uint8_t* g_rs_port = (volatile uint8_t*)0;
static volatile uint8_t* g_rs_tris = (volatile uint8_t*)0;
static volatile uint8_t* g_en_port = (volatile uint8_t*)0;
static volatile uint8_t* g_en_tris = (volatile uint8_t*)0;

static uint8_t g_rs_pin = 0u;
static uint8_t g_en_pin = 0u;
static uint8_t g_d4_pin = 0u;
static uint8_t g_d5_pin = 0u;
static uint8_t g_d6_pin = 0u;
static uint8_t g_d7_pin = 0u;

static void lcd_set_pin(volatile uint8_t* reg, uint8_t pin, uint8_t value)
{
    if (value != 0u)
    {
        SET_BIT((*reg), pin);
    }
    else
    {
        CLEAR_BIT((*reg), pin);
    }
}

static void lcd_pulse_enable(void)
{
    lcd_set_pin(g_en_port, g_en_pin, 1u);
    delay_us(2u);
    lcd_set_pin(g_en_port, g_en_pin, 0u);
    delay_us(2u);
}

static void lcd_send_nibble(uint8_t data)
{
    lcd_set_pin(g_data_port, g_d4_pin, (uint8_t)((data >> 0u) & 0x01u));
    lcd_set_pin(g_data_port, g_d5_pin, (uint8_t)((data >> 1u) & 0x01u));
    lcd_set_pin(g_data_port, g_d6_pin, (uint8_t)((data >> 2u) & 0x01u));
    lcd_set_pin(g_data_port, g_d7_pin, (uint8_t)((data >> 3u) & 0x01u));
    lcd_pulse_enable();
}

static void lcd_send_byte(uint8_t data, uint8_t rs)
{
    lcd_set_pin(g_rs_port, g_rs_pin, rs);
    lcd_send_nibble((uint8_t)(data >> 4u));
    lcd_send_nibble((uint8_t)(data & 0x0Fu));

    if (data == 0x01u || data == 0x02u)
    {
        delay_ms(2u);
    }
    else
    {
        delay_us(40u);
    }
}

void lcd_init_pins(
    volatile uint8_t* data_port,
    volatile uint8_t* data_tris,
    volatile uint8_t* rs_port,
    volatile uint8_t* rs_tris,
    uint8_t rs_pin,
    volatile uint8_t* en_port,
    volatile uint8_t* en_tris,
    uint8_t en_pin,
    uint8_t d4_pin,
    uint8_t d5_pin,
    uint8_t d6_pin,
    uint8_t d7_pin
)
{
    g_data_port = data_port;
    g_data_tris = data_tris;
    g_rs_port = rs_port;
    g_rs_tris = rs_tris;
    g_en_port = en_port;
    g_en_tris = en_tris;

    g_rs_pin = rs_pin;
    g_en_pin = en_pin;
    g_d4_pin = d4_pin;
    g_d5_pin = d5_pin;
    g_d6_pin = d6_pin;
    g_d7_pin = d7_pin;

    CLEAR_BIT((*g_data_tris), g_d4_pin);
    CLEAR_BIT((*g_data_tris), g_d5_pin);
    CLEAR_BIT((*g_data_tris), g_d6_pin);
    CLEAR_BIT((*g_data_tris), g_d7_pin);

    CLEAR_BIT((*g_rs_tris), g_rs_pin);
    CLEAR_BIT((*g_en_tris), g_en_pin);

    lcd_set_pin(g_rs_port, g_rs_pin, 0u);
    lcd_set_pin(g_en_port, g_en_pin, 0u);
}

void lcd_init(void)
{
    delay_ms(20u);

    lcd_set_pin(g_rs_port, g_rs_pin, 0u);

    lcd_send_nibble(0x03u);
    delay_ms(5u);
    lcd_send_nibble(0x03u);
    delay_us(150u);
    lcd_send_nibble(0x03u);
    delay_us(150u);
    lcd_send_nibble(0x02u);
    delay_us(150u);

    lcd_send_byte(0x28u, 0u);
    lcd_send_byte(0x0Cu, 0u);
    lcd_send_byte(0x01u, 0u);
    lcd_send_byte(0x06u, 0u);
}

void lcd_clear(void)
{
    lcd_send_byte(0x01u, 0u);
}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t base_addr;

    if (row == 0u)
    {
        base_addr = 0x00u;
    }
    else
    {
        base_addr = 0x40u;
    }

    lcd_send_byte((uint8_t)(0x80u | (base_addr + col)), 0u);
}

void lcd_write_char(char c)
{
    lcd_send_byte((uint8_t)c, 1u);
}

void lcd_write_string(const char* str)
{
    if (str == (const char*)0)
    {
        return;
    }

    while (*str != '\0')
    {
        lcd_write_char(*str);
        str++;
    }
}


