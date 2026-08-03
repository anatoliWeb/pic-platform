/*
 * File: libraries/display/lcd_hd44780/lcd_i2c.c
 *
 * PCF8574 I2C transport for the HD44780 controller. The pin mapping is
 * configured at compile time in lcd_i2c.h.
 *
 * Every I2C transaction reports a NACK through lcd_i2c_last_status() and
 * clears the ready flag. A NACK short-circuits the current operation: no
 * further I2C transactions or command delays run after the first failure.
 * Display operations refuse to touch the bus (LCD_I2C_NOT_INITIALIZED) until
 * the controller is initialized again through lcd_i2c_init() or
 * lcd_i2c_controller_init().
 */

#include "libraries/display/lcd_hd44780/lcd_i2c.h"

#include "core/delay.h"
#include "drivers/communication/i2c/i2c.h"

/* =========================================================
 * Compile-time validation of the pin mapping
 * ========================================================= */

#if (LCD_I2C_PIN_RS | LCD_I2C_PIN_RW | LCD_I2C_PIN_EN | LCD_I2C_PIN_BL) > 0xFFu
    #error "LCD_I2C control pins must fit in 8 bits"
#endif

#if LCD_I2C_PIN_RS == 0u
    #error "LCD_I2C_PIN_RS must be non-zero"
#endif
#if LCD_I2C_PIN_RW == 0u
    #error "LCD_I2C_PIN_RW must be non-zero"
#endif
#if LCD_I2C_PIN_EN == 0u
    #error "LCD_I2C_PIN_EN must be non-zero"
#endif
#if LCD_I2C_PIN_BL == 0u
    #error "LCD_I2C_PIN_BL must be non-zero"
#endif

#if (LCD_I2C_PIN_RS & (LCD_I2C_PIN_RS - 1u)) != 0u
    #error "LCD_I2C_PIN_RS must be a single-bit mask"
#endif
#if (LCD_I2C_PIN_RW & (LCD_I2C_PIN_RW - 1u)) != 0u
    #error "LCD_I2C_PIN_RW must be a single-bit mask"
#endif
#if (LCD_I2C_PIN_EN & (LCD_I2C_PIN_EN - 1u)) != 0u
    #error "LCD_I2C_PIN_EN must be a single-bit mask"
#endif
#if (LCD_I2C_PIN_BL & (LCD_I2C_PIN_BL - 1u)) != 0u
    #error "LCD_I2C_PIN_BL must be a single-bit mask"
#endif

#if (LCD_I2C_PIN_RS & (LCD_I2C_PIN_RW | LCD_I2C_PIN_EN | LCD_I2C_PIN_BL)) != 0u
    #error "LCD_I2C_PIN_RS overlaps another control pin"
#endif
#if (LCD_I2C_PIN_RW & (LCD_I2C_PIN_RS | LCD_I2C_PIN_EN | LCD_I2C_PIN_BL)) != 0u
    #error "LCD_I2C_PIN_RW overlaps another control pin"
#endif
#if (LCD_I2C_PIN_EN & (LCD_I2C_PIN_RS | LCD_I2C_PIN_RW | LCD_I2C_PIN_BL)) != 0u
    #error "LCD_I2C_PIN_EN overlaps another control pin"
#endif
#if (LCD_I2C_PIN_BL & (LCD_I2C_PIN_RS | LCD_I2C_PIN_RW | LCD_I2C_PIN_EN)) != 0u
    #error "LCD_I2C_PIN_BL overlaps another control pin"
#endif

#if (LCD_I2C_DATA_SHIFT + 4u) > 8u
    #error "LCD_I2C_DATA_SHIFT + 4 must not exceed 8"
#endif

#if ((0x0Fu << LCD_I2C_DATA_SHIFT) & (LCD_I2C_PIN_RS | LCD_I2C_PIN_RW | LCD_I2C_PIN_EN | LCD_I2C_PIN_BL)) != 0u
    #error "LCD_I2C data pins overlap the control pins"
#endif

/* =========================================================
 * Module state
 * ========================================================= */

/* Address 0 is reserved as the "not configured" sentinel. */
static uint8_t g_i2c_addr = 0u;
static uint8_t g_backlight = LCD_I2C_PIN_BL;
static uint8_t g_ready = 0u;
static lcd_i2c_status_t g_last_status = LCD_I2C_OK;

/* =========================================================
 * Low-level transport
 * ========================================================= */

static lcd_i2c_status_t lcd_i2c_send(uint8_t data)
{
    uint8_t nack;

    if (g_i2c_addr == 0u)
    {
        g_last_status = LCD_I2C_NOT_INITIALIZED;
        return LCD_I2C_NOT_INITIALIZED;
    }

    i2c_start();
    nack = i2c_write_byte((uint8_t)(g_i2c_addr << 1u));
    if (nack == 0u)
    {
        nack = i2c_write_byte(data);
    }
    i2c_stop();

    if (nack != 0u)
    {
        g_ready = 0u;
        g_last_status = LCD_I2C_NO_ACK;
        return LCD_I2C_NO_ACK;
    }

    g_last_status = LCD_I2C_OK;
    return LCD_I2C_OK;
}

static lcd_i2c_status_t lcd_i2c_pulse(uint8_t data)
{
    lcd_i2c_status_t status;

    status = lcd_i2c_send((uint8_t)(data | LCD_I2C_PIN_EN));
    if (status != LCD_I2C_OK)
    {
        return status;
    }
    delay_us(2u);

    status = lcd_i2c_send((uint8_t)(data & (uint8_t)(~LCD_I2C_PIN_EN)));
    if (status != LCD_I2C_OK)
    {
        return status;
    }
    delay_us(2u);

    return LCD_I2C_OK;
}

static lcd_i2c_status_t lcd_i2c_nibble(uint8_t nibble, uint8_t rs)
{
    uint8_t out = g_backlight;

    if (rs != 0u)
    {
        out |= LCD_I2C_PIN_RS;
    }

    out |= (uint8_t)((nibble & 0x0Fu) << LCD_I2C_DATA_SHIFT);
    return lcd_i2c_pulse(out);
}

static lcd_i2c_status_t lcd_i2c_byte(uint8_t data, uint8_t rs)
{
    lcd_i2c_status_t status;

    status = lcd_i2c_nibble((uint8_t)(data >> 4u), rs);
    if (status != LCD_I2C_OK)
    {
        return status;
    }

    status = lcd_i2c_nibble((uint8_t)(data & 0x0Fu), rs);
    if (status != LCD_I2C_OK)
    {
        return status;
    }

    if (data == 0x01u || data == 0x02u)
    {
        delay_ms(2u);
    }
    else
    {
        delay_us(40u);
    }

    return LCD_I2C_OK;
}

static lcd_i2c_status_t lcd_hd44780_init_sequence(void)
{
    lcd_i2c_status_t status;

    delay_ms(20u);

    status = lcd_i2c_nibble(0x03u, 0u);
    if (status != LCD_I2C_OK)
    {
        return status;
    }
    delay_ms(5u);

    status = lcd_i2c_nibble(0x03u, 0u);
    if (status != LCD_I2C_OK)
    {
        return status;
    }
    delay_us(150u);

    status = lcd_i2c_nibble(0x03u, 0u);
    if (status != LCD_I2C_OK)
    {
        return status;
    }
    delay_us(150u);

    status = lcd_i2c_nibble(0x02u, 0u);
    if (status != LCD_I2C_OK)
    {
        return status;
    }
    delay_us(150u);

    status = lcd_i2c_byte(0x28u, 0u);
    if (status != LCD_I2C_OK)
    {
        return status;
    }
    status = lcd_i2c_byte(0x0Cu, 0u);
    if (status != LCD_I2C_OK)
    {
        return status;
    }
    status = lcd_i2c_byte(0x01u, 0u);
    if (status != LCD_I2C_OK)
    {
        return status;
    }
    status = lcd_i2c_byte(0x06u, 0u);

    return status;
}

/* =========================================================
 * Ready gate for display operations
 * ========================================================= */

/*
 * Display operations refuse to run until the controller is initialized and
 * the last transaction succeeded. This covers both "no init yet" and
 * "a previous NACK cleared the ready flag": recovery is an explicit
 * lcd_i2c_init() or lcd_i2c_controller_init() call.
 */
static lcd_i2c_status_t lcd_i2c_check_ready(void)
{
    if (g_ready == 0u)
    {
        g_last_status = LCD_I2C_NOT_INITIALIZED;
        return LCD_I2C_NOT_INITIALIZED;
    }

    return LCD_I2C_OK;
}

/* =========================================================
 * Init / ownership API
 * ========================================================= */

lcd_i2c_status_t lcd_i2c_init(uint8_t i2c_addr, uint32_t i2c_clock_hz)
{
    if ((i2c_addr == 0u) || (i2c_addr > 0x7Fu))
    {
        g_last_status = LCD_I2C_INVALID_ARGUMENT;
        return LCD_I2C_INVALID_ARGUMENT;
    }

    g_i2c_addr = i2c_addr;
    g_backlight = LCD_I2C_PIN_BL;
    g_ready = 0u;
    g_last_status = LCD_I2C_OK;

    i2c_init(i2c_clock_hz);
    g_last_status = lcd_hd44780_init_sequence();
    g_ready = (g_last_status == LCD_I2C_OK) ? 1u : 0u;

    return g_last_status;
}

lcd_i2c_status_t lcd_i2c_attach(uint8_t i2c_addr)
{
    lcd_i2c_status_t status;

    status = lcd_i2c_probe(i2c_addr);
    if (status != LCD_I2C_OK)
    {
        return status;
    }

    g_i2c_addr = i2c_addr;
    g_backlight = LCD_I2C_PIN_BL;
    g_ready = 0u;
    g_last_status = LCD_I2C_OK;

    return LCD_I2C_OK;
}

lcd_i2c_status_t lcd_i2c_controller_init(void)
{
    if (g_i2c_addr == 0u)
    {
        g_last_status = LCD_I2C_NOT_INITIALIZED;
        return LCD_I2C_NOT_INITIALIZED;
    }

    g_ready = 0u;
    g_last_status = LCD_I2C_OK;

    g_last_status = lcd_hd44780_init_sequence();
    g_ready = (g_last_status == LCD_I2C_OK) ? 1u : 0u;

    return g_last_status;
}

lcd_i2c_status_t lcd_i2c_probe(uint8_t i2c_addr)
{
    uint8_t nack;

    if ((i2c_addr == 0u) || (i2c_addr > 0x7Fu))
    {
        g_last_status = LCD_I2C_INVALID_ARGUMENT;
        return LCD_I2C_INVALID_ARGUMENT;
    }

    i2c_start();
    nack = i2c_write_byte((uint8_t)(i2c_addr << 1u));
    i2c_stop();

    if (nack != 0u)
    {
        g_last_status = LCD_I2C_NO_ACK;
        return LCD_I2C_NO_ACK;
    }

    g_last_status = LCD_I2C_OK;
    return LCD_I2C_OK;
}

lcd_i2c_status_t lcd_i2c_last_status(void)
{
    return g_last_status;
}

uint8_t lcd_i2c_is_ready(void)
{
    return g_ready;
}

/* =========================================================
 * Display operations
 * ========================================================= */

void lcd_i2c_clear(void)
{
    if (lcd_i2c_check_ready() != LCD_I2C_OK)
    {
        return;
    }

    lcd_i2c_byte(0x01u, 0u);
}

void lcd_i2c_home(void)
{
    if (lcd_i2c_check_ready() != LCD_I2C_OK)
    {
        return;
    }

    lcd_i2c_byte(0x02u, 0u);
}

void lcd_i2c_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t base;

    if (lcd_i2c_check_ready() != LCD_I2C_OK)
    {
        return;
    }

    if (row > 1u)
    {
        g_last_status = LCD_I2C_INVALID_ARGUMENT;
        return;
    }

    if (col > 0x27u)
    {
        g_last_status = LCD_I2C_INVALID_ARGUMENT;
        return;
    }

    base = (row == 0u) ? 0x00u : 0x40u;
    lcd_i2c_byte((uint8_t)(0x80u | (uint8_t)(base + col)), 0u);
}

void lcd_i2c_write_char(char c)
{
    if (lcd_i2c_check_ready() != LCD_I2C_OK)
    {
        return;
    }

    lcd_i2c_byte((uint8_t)c, 1u);
}

void lcd_i2c_write_string(const char* str)
{
    if (lcd_i2c_check_ready() != LCD_I2C_OK)
    {
        return;
    }

    if (str == (const char*)0)
    {
        g_last_status = LCD_I2C_INVALID_ARGUMENT;
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
    if (lcd_i2c_check_ready() != LCD_I2C_OK)
    {
        return;
    }

    if ((on != 0u) && (on != 1u))
    {
        g_last_status = LCD_I2C_INVALID_ARGUMENT;
        return;
    }

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
