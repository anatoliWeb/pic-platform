#include "libraries/time/ds1302/ds1302.h"

#include "core/delay.h"
#include "drivers/gpio/gpio.h"

#define DS1302_REG_SECONDS_W 0x80u
#define DS1302_REG_SECONDS_R 0x81u
#define DS1302_REG_MINUTES_W 0x82u
#define DS1302_REG_MINUTES_R 0x83u
#define DS1302_REG_HOURS_W   0x84u
#define DS1302_REG_HOURS_R   0x85u
#define DS1302_REG_DATE_W    0x86u
#define DS1302_REG_DATE_R    0x87u
#define DS1302_REG_MONTH_W   0x88u
#define DS1302_REG_MONTH_R   0x89u
#define DS1302_REG_DAY_W     0x8Au
#define DS1302_REG_DAY_R     0x8Bu
#define DS1302_REG_YEAR_W    0x8Cu
#define DS1302_REG_YEAR_R    0x8Du
#define DS1302_REG_WP_W      0x8Eu
#define DS1302_REG_WP_R      0x8Fu

#define DS1302_RAM_BASE_W    0xC0u
#define DS1302_RAM_BASE_R    0xC1u
#define DS1302_RAM_SIZE      31u

#define DS1302_CH_MASK       0x80u
#define DS1302_WP_MASK       0x80u

static uint8_t ds1302_bin_to_bcd(uint8_t value)
{
    return (uint8_t)(((value / 10u) << 4u) | (value % 10u));
}

static uint8_t ds1302_bcd_to_bin(uint8_t value)
{
    return (uint8_t)(((value >> 4u) * 10u) + (value & 0x0Fu));
}

static void ds1302_io_output(ds1302_t* rtc)
{
    gpio_set_output(rtc->io_tris, rtc->io_pin);
}

static void ds1302_io_input(ds1302_t* rtc)
{
    gpio_set_input(rtc->io_tris, rtc->io_pin);
}

static void ds1302_ce_high(ds1302_t* rtc) { gpio_write_high(rtc->ce_port, rtc->ce_pin); }
static void ds1302_ce_low(ds1302_t* rtc) { gpio_write_low(rtc->ce_port, rtc->ce_pin); }
static void ds1302_clk_high(ds1302_t* rtc) { gpio_write_high(rtc->clk_port, rtc->clk_pin); }
static void ds1302_clk_low(ds1302_t* rtc) { gpio_write_low(rtc->clk_port, rtc->clk_pin); }
static void ds1302_io_high(ds1302_t* rtc) { gpio_write_high(rtc->io_port, rtc->io_pin); }
static void ds1302_io_low(ds1302_t* rtc) { gpio_write_low(rtc->io_port, rtc->io_pin); }
static uint8_t ds1302_io_read(ds1302_t* rtc) { return gpio_read(rtc->io_port, rtc->io_pin); }

static void ds1302_delay(void)
{
    DRV_DELAY_US(1u);
}

static void ds1302_write_byte(ds1302_t* rtc, uint8_t value)
{
    uint8_t i;

    ds1302_io_output(rtc);
    for (i = 0u; i < 8u; i++)
    {
        if ((value & 0x01u) != 0u) { ds1302_io_high(rtc); }
        else { ds1302_io_low(rtc); }

        ds1302_delay();
        ds1302_clk_high(rtc);
        ds1302_delay();
        ds1302_clk_low(rtc);
        ds1302_delay();

        value >>= 1u;
    }
}

static uint8_t ds1302_read_byte(ds1302_t* rtc)
{
    uint8_t i;
    uint8_t value;

    value = 0u;
    ds1302_io_input(rtc);

    for (i = 0u; i < 8u; i++)
    {
        ds1302_clk_high(rtc);
        ds1302_delay();
        if (ds1302_io_read(rtc) != 0u)
        {
            value |= (uint8_t)(1u << i);
        }
        ds1302_clk_low(rtc);
        ds1302_delay();
    }

    return value;
}

static void ds1302_write_reg(ds1302_t* rtc, uint8_t cmd_write, uint8_t value)
{
    ds1302_ce_high(rtc);
    ds1302_delay();
    ds1302_write_byte(rtc, cmd_write);
    ds1302_write_byte(rtc, value);
    ds1302_ce_low(rtc);
    ds1302_delay();
}

static uint8_t ds1302_read_reg(ds1302_t* rtc, uint8_t cmd_read)
{
    uint8_t value;
    ds1302_ce_high(rtc);
    ds1302_delay();
    ds1302_write_byte(rtc, cmd_read);
    value = ds1302_read_byte(rtc);
    ds1302_ce_low(rtc);
    ds1302_delay();
    return value;
}

static uint8_t ds1302_set_datetime_backend(void* ctx, const rtc_datetime_t* dt)
{
    return (uint8_t)(ds1302_set_datetime((ds1302_t*)ctx, dt) == DS1302_STATUS_OK ? 1u : 0u);
}

static uint8_t ds1302_get_datetime_backend(void* ctx, rtc_datetime_t* dt)
{
    return (uint8_t)(ds1302_get_datetime((ds1302_t*)ctx, dt) == DS1302_STATUS_OK ? 1u : 0u);
}

static const rtc_driver_t g_ds1302_driver =
{
    ds1302_set_datetime_backend,
    ds1302_get_datetime_backend
};

uint8_t ds1302_init(ds1302_t* rtc)
{
    if (rtc == (ds1302_t*)0)
    {
        return 0u;
    }

    gpio_set_output(rtc->ce_tris, rtc->ce_pin);
    gpio_set_output(rtc->clk_tris, rtc->clk_pin);
    gpio_set_output(rtc->io_tris, rtc->io_pin);

    ds1302_ce_low(rtc);
    ds1302_clk_low(rtc);
    ds1302_io_low(rtc);

    rtc->initialized = 1u;
    return 1u;
}

void ds1302_enable_write_protect(ds1302_t* rtc)
{
    if ((rtc == (ds1302_t*)0) || (rtc->initialized == 0u))
    {
        return;
    }
    ds1302_write_reg(rtc, DS1302_REG_WP_W, DS1302_WP_MASK);
}

void ds1302_disable_write_protect(ds1302_t* rtc)
{
    if ((rtc == (ds1302_t*)0) || (rtc->initialized == 0u))
    {
        return;
    }
    ds1302_write_reg(rtc, DS1302_REG_WP_W, 0x00u);
}

ds1302_status_t ds1302_set_datetime(ds1302_t* rtc, const rtc_datetime_t* dt)
{
    uint8_t wd;

    if ((rtc == (ds1302_t*)0) || (dt == (const rtc_datetime_t*)0) || (rtc->initialized == 0u))
    {
        return DS1302_STATUS_ERROR;
    }

    if (rtc_is_valid(dt) == 0u)
    {
        return DS1302_STATUS_INVALID_DATETIME;
    }

    wd = (uint8_t)((dt->weekday < 1u || dt->weekday > 7u) ? 1u : dt->weekday);

    ds1302_disable_write_protect(rtc);

    ds1302_write_reg(rtc, DS1302_REG_SECONDS_W, (uint8_t)(ds1302_bin_to_bcd(dt->second) & 0x7Fu));
    ds1302_write_reg(rtc, DS1302_REG_MINUTES_W, ds1302_bin_to_bcd(dt->minute));
    ds1302_write_reg(rtc, DS1302_REG_HOURS_W, ds1302_bin_to_bcd(dt->hour));
    ds1302_write_reg(rtc, DS1302_REG_DATE_W, ds1302_bin_to_bcd(dt->day));
    ds1302_write_reg(rtc, DS1302_REG_MONTH_W, ds1302_bin_to_bcd(dt->month));
    ds1302_write_reg(rtc, DS1302_REG_DAY_W, ds1302_bin_to_bcd(wd));
    ds1302_write_reg(rtc, DS1302_REG_YEAR_W, ds1302_bin_to_bcd((uint8_t)(dt->year % 100u)));

    ds1302_enable_write_protect(rtc);

    return DS1302_STATUS_OK;
}

ds1302_status_t ds1302_get_datetime(ds1302_t* rtc, rtc_datetime_t* dt)
{
    uint8_t sec;
    uint8_t min;
    uint8_t hr;
    uint8_t day;
    uint8_t mon;
    uint8_t wday;
    uint8_t year;

    if ((rtc == (ds1302_t*)0) || (dt == (rtc_datetime_t*)0) || (rtc->initialized == 0u))
    {
        return DS1302_STATUS_ERROR;
    }

    sec = ds1302_read_reg(rtc, DS1302_REG_SECONDS_R);
    if ((sec & DS1302_CH_MASK) != 0u)
    {
        return DS1302_STATUS_CLOCK_HALTED;
    }

    min = ds1302_read_reg(rtc, DS1302_REG_MINUTES_R);
    hr = ds1302_read_reg(rtc, DS1302_REG_HOURS_R);
    day = ds1302_read_reg(rtc, DS1302_REG_DATE_R);
    mon = ds1302_read_reg(rtc, DS1302_REG_MONTH_R);
    wday = ds1302_read_reg(rtc, DS1302_REG_DAY_R);
    year = ds1302_read_reg(rtc, DS1302_REG_YEAR_R);

    dt->second = ds1302_bcd_to_bin((uint8_t)(sec & 0x7Fu));
    dt->minute = ds1302_bcd_to_bin((uint8_t)(min & 0x7Fu));
    dt->hour = ds1302_bcd_to_bin((uint8_t)(hr & 0x3Fu));
    dt->day = ds1302_bcd_to_bin((uint8_t)(day & 0x3Fu));
    dt->month = ds1302_bcd_to_bin((uint8_t)(mon & 0x1Fu));
    dt->weekday = ds1302_bcd_to_bin((uint8_t)(wday & 0x07u));
    dt->year = (uint16_t)(2000u + ds1302_bcd_to_bin(year));

    if (rtc_is_valid(dt) == 0u)
    {
        return DS1302_STATUS_INVALID_DATETIME;
    }

    return DS1302_STATUS_OK;
}

ds1302_status_t ds1302_write_ram(ds1302_t* rtc, uint8_t offset, uint8_t value)
{
    uint8_t cmd;

    if ((rtc == (ds1302_t*)0) || (rtc->initialized == 0u))
    {
        return DS1302_STATUS_ERROR;
    }

    if (offset >= DS1302_RAM_SIZE)
    {
        return DS1302_STATUS_INVALID_RAM_ADDRESS;
    }

    cmd = (uint8_t)(DS1302_RAM_BASE_W + (uint8_t)(offset << 1u));
    ds1302_disable_write_protect(rtc);
    ds1302_write_reg(rtc, cmd, value);
    ds1302_enable_write_protect(rtc);

    return DS1302_STATUS_OK;
}

ds1302_status_t ds1302_read_ram(ds1302_t* rtc, uint8_t offset, uint8_t* value)
{
    uint8_t cmd;

    if ((rtc == (ds1302_t*)0) || (value == (uint8_t*)0) || (rtc->initialized == 0u))
    {
        return DS1302_STATUS_ERROR;
    }

    if (offset >= DS1302_RAM_SIZE)
    {
        return DS1302_STATUS_INVALID_RAM_ADDRESS;
    }

    cmd = (uint8_t)(DS1302_RAM_BASE_R + (uint8_t)(offset << 1u));
    *value = ds1302_read_reg(rtc, cmd);
    return DS1302_STATUS_OK;
}

const rtc_driver_t* ds1302_get_driver(void)
{
    return &g_ds1302_driver;
}
