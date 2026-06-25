/*
 * File: libraries/time/ds1307/ds1307.c
 */

#include "libraries/time/ds1307/ds1307.h"

#include "drivers/communication/i2c/i2c.h"

#define DS1307_REG_SECONDS   0x00u
#define DS1307_REG_MINUTES   0x01u
#define DS1307_REG_HOURS     0x02u
#define DS1307_REG_WEEKDAY   0x03u
#define DS1307_REG_DAY       0x04u
#define DS1307_REG_MONTH     0x05u
#define DS1307_REG_YEAR      0x06u
#define DS1307_REG_CONTROL   0x07u

#define DS1307_RAM_BASE      0x08u
#define DS1307_RAM_SIZE      56u

#define DS1307_CH_MASK       0x80u

static uint8_t ds1307_bin_to_bcd(uint8_t value)
{
    return (uint8_t)(((value / 10u) << 4u) | (value % 10u));
}

static uint8_t ds1307_bcd_to_bin(uint8_t value)
{
    return (uint8_t)(((value >> 4u) * 10u) + (value & 0x0Fu));
}

static uint8_t ds1307_read_u8(ds1307_t* rtc, uint8_t reg, uint8_t* value)
{
    return i2c_read_register(rtc->i2c_address, reg, value);
}

static uint8_t ds1307_write_u8(ds1307_t* rtc, uint8_t reg, uint8_t value)
{
    return i2c_write_register(rtc->i2c_address, reg, value);
}

static uint8_t ds1307_read_buf(ds1307_t* rtc, uint8_t reg, uint8_t* buf, uint8_t len)
{
    uint8_t i;

    if (buf == (uint8_t*)0)
    {
        return 1u;
    }

    i2c_start();
    if (i2c_write_byte((uint8_t)(rtc->i2c_address << 1u)) != 0u) { i2c_stop(); return 1u; }
    if (i2c_write_byte(reg) != 0u) { i2c_stop(); return 1u; }
    i2c_restart();
    if (i2c_write_byte((uint8_t)((rtc->i2c_address << 1u) | 0x01u)) != 0u) { i2c_stop(); return 1u; }

    for (i = 0u; i < len; i++)
    {
        buf[i] = i2c_read_byte((uint8_t)(i < (uint8_t)(len - 1u) ? 1u : 0u));
    }

    i2c_stop();
    return 0u;
}

static uint8_t ds1307_set_datetime_backend(void* ctx, const rtc_datetime_t* dt)
{
    return (uint8_t)(ds1307_set_datetime((ds1307_t*)ctx, dt) == DS1307_STATUS_OK ? 1u : 0u);
}

static uint8_t ds1307_get_datetime_backend(void* ctx, rtc_datetime_t* dt)
{
    return (uint8_t)(ds1307_get_datetime((ds1307_t*)ctx, dt) == DS1307_STATUS_OK ? 1u : 0u);
}

static const rtc_driver_t g_ds1307_driver =
{
    ds1307_set_datetime_backend,
    ds1307_get_datetime_backend
};

uint8_t ds1307_init(ds1307_t* rtc, uint8_t i2c_address)
{
    uint8_t sec;

    if (rtc == (ds1307_t*)0)
    {
        return 0u;
    }

    rtc->i2c_address = i2c_address;
    rtc->initialized = 0u;

    if (ds1307_read_u8(rtc, DS1307_REG_SECONDS, &sec) != 0u)
    {
        return 0u;
    }

    rtc->initialized = 1u;
    return 1u;
}

ds1307_status_t ds1307_set_datetime(ds1307_t* rtc, const rtc_datetime_t* dt)
{
    uint8_t d[7];

    if ((rtc == (ds1307_t*)0) || (dt == (const rtc_datetime_t*)0) || (rtc->initialized == 0u))
    {
        return DS1307_STATUS_ERROR;
    }

    if (rtc_is_valid(dt) == 0u)
    {
        return DS1307_STATUS_INVALID_DATETIME;
    }

    d[0] = (uint8_t)(ds1307_bin_to_bcd(dt->second) & 0x7Fu); /* CH bit cleared => run oscillator */
    d[1] = (uint8_t)(ds1307_bin_to_bcd(dt->minute) & 0x7Fu);
    d[2] = (uint8_t)(ds1307_bin_to_bcd(dt->hour) & 0x3Fu);
    d[3] = (uint8_t)(ds1307_bin_to_bcd((uint8_t)((dt->weekday < 1u || dt->weekday > 7u) ? 1u : dt->weekday)) & 0x07u);
    d[4] = (uint8_t)(ds1307_bin_to_bcd(dt->day) & 0x3Fu);
    d[5] = (uint8_t)(ds1307_bin_to_bcd(dt->month) & 0x1Fu);
    d[6] = ds1307_bin_to_bcd((uint8_t)(dt->year % 100u));

    if (ds1307_write_u8(rtc, DS1307_REG_SECONDS, d[0]) != 0u) { return DS1307_STATUS_I2C_ERROR; }
    if (ds1307_write_u8(rtc, DS1307_REG_MINUTES, d[1]) != 0u) { return DS1307_STATUS_I2C_ERROR; }
    if (ds1307_write_u8(rtc, DS1307_REG_HOURS, d[2]) != 0u) { return DS1307_STATUS_I2C_ERROR; }
    if (ds1307_write_u8(rtc, DS1307_REG_WEEKDAY, d[3]) != 0u) { return DS1307_STATUS_I2C_ERROR; }
    if (ds1307_write_u8(rtc, DS1307_REG_DAY, d[4]) != 0u) { return DS1307_STATUS_I2C_ERROR; }
    if (ds1307_write_u8(rtc, DS1307_REG_MONTH, d[5]) != 0u) { return DS1307_STATUS_I2C_ERROR; }
    if (ds1307_write_u8(rtc, DS1307_REG_YEAR, d[6]) != 0u) { return DS1307_STATUS_I2C_ERROR; }

    return DS1307_STATUS_OK;
}

ds1307_status_t ds1307_get_datetime(ds1307_t* rtc, rtc_datetime_t* dt)
{
    uint8_t d[7];

    if ((rtc == (ds1307_t*)0) || (dt == (rtc_datetime_t*)0) || (rtc->initialized == 0u))
    {
        return DS1307_STATUS_ERROR;
    }

    if (ds1307_read_buf(rtc, DS1307_REG_SECONDS, d, 7u) != 0u)
    {
        return DS1307_STATUS_I2C_ERROR;
    }

    if ((d[0] & DS1307_CH_MASK) != 0u)
    {
        return DS1307_STATUS_OSCILLATOR_HALTED;
    }

    dt->second = ds1307_bcd_to_bin((uint8_t)(d[0] & 0x7Fu));
    dt->minute = ds1307_bcd_to_bin((uint8_t)(d[1] & 0x7Fu));
    dt->hour = ds1307_bcd_to_bin((uint8_t)(d[2] & 0x3Fu));
    dt->weekday = ds1307_bcd_to_bin((uint8_t)(d[3] & 0x07u));
    dt->day = ds1307_bcd_to_bin((uint8_t)(d[4] & 0x3Fu));
    dt->month = ds1307_bcd_to_bin((uint8_t)(d[5] & 0x1Fu));
    dt->year = (uint16_t)(2000u + ds1307_bcd_to_bin(d[6]));

    if (rtc_is_valid(dt) == 0u)
    {
        return DS1307_STATUS_INVALID_DATETIME;
    }

    return DS1307_STATUS_OK;
}

ds1307_status_t ds1307_set_ram(ds1307_t* rtc, uint8_t offset, uint8_t value)
{
    if ((rtc == (ds1307_t*)0) || (rtc->initialized == 0u))
    {
        return DS1307_STATUS_ERROR;
    }

    if (offset >= DS1307_RAM_SIZE)
    {
        return DS1307_STATUS_INVALID_RAM_ADDRESS;
    }

    if (ds1307_write_u8(rtc, (uint8_t)(DS1307_RAM_BASE + offset), value) != 0u)
    {
        return DS1307_STATUS_I2C_ERROR;
    }

    return DS1307_STATUS_OK;
}

ds1307_status_t ds1307_get_ram(ds1307_t* rtc, uint8_t offset, uint8_t* value)
{
    if ((rtc == (ds1307_t*)0) || (rtc->initialized == 0u) || (value == (uint8_t*)0))
    {
        return DS1307_STATUS_ERROR;
    }

    if (offset >= DS1307_RAM_SIZE)
    {
        return DS1307_STATUS_INVALID_RAM_ADDRESS;
    }

    if (ds1307_read_u8(rtc, (uint8_t)(DS1307_RAM_BASE + offset), value) != 0u)
    {
        return DS1307_STATUS_I2C_ERROR;
    }

    return DS1307_STATUS_OK;
}

const rtc_driver_t* ds1307_get_driver(void)
{
    return &g_ds1307_driver;
}
