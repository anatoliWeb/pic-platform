/*
 * File: libraries/time/ds3231/ds3231.c
 */

#include "libraries/time/ds3231/ds3231.h"

#include "drivers/communication/i2c/i2c.h"

#define DS3231_REG_SECONDS      0x00u
#define DS3231_REG_MINUTES      0x01u
#define DS3231_REG_HOURS        0x02u
#define DS3231_REG_WEEKDAY      0x03u
#define DS3231_REG_DATE         0x04u
#define DS3231_REG_MONTH        0x05u
#define DS3231_REG_YEAR         0x06u
#define DS3231_REG_CONTROL      0x0Eu
#define DS3231_REG_STATUS       0x0Fu
#define DS3231_REG_TEMP_MSB     0x11u
#define DS3231_REG_TEMP_LSB     0x12u

#define DS3231_STATUS_OSF_MASK  0x80u

static uint8_t ds3231_bin_to_bcd(uint8_t value)
{
    return (uint8_t)(((value / 10u) << 4u) | (value % 10u));
}

static uint8_t ds3231_bcd_to_bin(uint8_t value)
{
    return (uint8_t)(((value >> 4u) * 10u) + (value & 0x0Fu));
}

static uint8_t ds3231_read_u8(ds3231_t* rtc, uint8_t reg, uint8_t* value)
{
    return i2c_read_register(rtc->i2c_address, reg, value);
}

static uint8_t ds3231_write_u8(ds3231_t* rtc, uint8_t reg, uint8_t value)
{
    return i2c_write_register(rtc->i2c_address, reg, value);
}

static uint8_t ds3231_read_buf(ds3231_t* rtc, uint8_t reg, uint8_t* buf, uint8_t len)
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

static ds3231_status_t ds3231_status_from_i2c(uint8_t i2c_result)
{
    if (i2c_result != 0u)
    {
        return DS3231_STATUS_I2C_ERROR;
    }
    return DS3231_STATUS_OK;
}

static uint8_t ds3231_validate_dt(const rtc_datetime_t* dt)
{
    return rtc_is_valid(dt);
}

static uint8_t ds3231_set_datetime_backend(void* ctx, const rtc_datetime_t* dt)
{
    ds3231_status_t st;
    st = ds3231_set_datetime((ds3231_t*)ctx, dt);
    return (uint8_t)(st == DS3231_STATUS_OK ? 1u : 0u);
}

static uint8_t ds3231_get_datetime_backend(void* ctx, rtc_datetime_t* dt)
{
    ds3231_status_t st;
    st = ds3231_get_datetime((ds3231_t*)ctx, dt);
    return (uint8_t)(st == DS3231_STATUS_OK ? 1u : 0u);
}

static const rtc_driver_t g_ds3231_driver =
{
    ds3231_set_datetime_backend,
    ds3231_get_datetime_backend
};

uint8_t ds3231_init(ds3231_t* rtc, uint8_t i2c_address)
{
    uint8_t status_reg;

    if (rtc == (ds3231_t*)0)
    {
        return 0u;
    }

    rtc->i2c_address = i2c_address;
    rtc->initialized = 0u;

    if (ds3231_read_u8(rtc, DS3231_REG_STATUS, &status_reg) != 0u)
    {
        return 0u;
    }

    rtc->initialized = 1u;
    return 1u;
}

ds3231_status_t ds3231_set_datetime(ds3231_t* rtc, const rtc_datetime_t* dt)
{
    uint8_t d[7];
    uint8_t status_reg;
    ds3231_status_t st;

    if ((rtc == (ds3231_t*)0) || (dt == (const rtc_datetime_t*)0) || (rtc->initialized == 0u))
    {
        return DS3231_STATUS_ERROR;
    }

    if (ds3231_validate_dt(dt) == 0u)
    {
        return DS3231_STATUS_INVALID_DATETIME;
    }

    d[0] = ds3231_bin_to_bcd(dt->second);
    d[1] = ds3231_bin_to_bcd(dt->minute);
    d[2] = ds3231_bin_to_bcd(dt->hour);
    d[3] = ds3231_bin_to_bcd((uint8_t)((dt->weekday < 1u || dt->weekday > 7u) ? 1u : dt->weekday));
    d[4] = ds3231_bin_to_bcd(dt->day);
    d[5] = ds3231_bin_to_bcd(dt->month);
    d[6] = ds3231_bin_to_bcd((uint8_t)(dt->year % 100u));

    st = ds3231_status_from_i2c(ds3231_write_u8(rtc, DS3231_REG_SECONDS, d[0]));
    if (st != DS3231_STATUS_OK) { return st; }
    st = ds3231_status_from_i2c(ds3231_write_u8(rtc, DS3231_REG_MINUTES, d[1]));
    if (st != DS3231_STATUS_OK) { return st; }
    st = ds3231_status_from_i2c(ds3231_write_u8(rtc, DS3231_REG_HOURS, d[2]));
    if (st != DS3231_STATUS_OK) { return st; }
    st = ds3231_status_from_i2c(ds3231_write_u8(rtc, DS3231_REG_WEEKDAY, d[3]));
    if (st != DS3231_STATUS_OK) { return st; }
    st = ds3231_status_from_i2c(ds3231_write_u8(rtc, DS3231_REG_DATE, d[4]));
    if (st != DS3231_STATUS_OK) { return st; }
    st = ds3231_status_from_i2c(ds3231_write_u8(rtc, DS3231_REG_MONTH, d[5]));
    if (st != DS3231_STATUS_OK) { return st; }
    st = ds3231_status_from_i2c(ds3231_write_u8(rtc, DS3231_REG_YEAR, d[6]));
    if (st != DS3231_STATUS_OK) { return st; }

    /* Clear oscillator stop flag after setting known valid time. */
    if (ds3231_read_u8(rtc, DS3231_REG_STATUS, &status_reg) == 0u)
    {
        status_reg = (uint8_t)(status_reg & (uint8_t)(~DS3231_STATUS_OSF_MASK));
        (void)ds3231_write_u8(rtc, DS3231_REG_STATUS, status_reg);
    }

    return DS3231_STATUS_OK;
}

ds3231_status_t ds3231_get_datetime(ds3231_t* rtc, rtc_datetime_t* dt)
{
    uint8_t d[7];
    uint8_t status_reg;
    uint16_t year2;

    if ((rtc == (ds3231_t*)0) || (dt == (rtc_datetime_t*)0) || (rtc->initialized == 0u))
    {
        return DS3231_STATUS_ERROR;
    }

    if (ds3231_read_u8(rtc, DS3231_REG_STATUS, &status_reg) != 0u)
    {
        return DS3231_STATUS_I2C_ERROR;
    }

    if ((status_reg & DS3231_STATUS_OSF_MASK) != 0u)
    {
        return DS3231_STATUS_OSCILLATOR_STOPPED;
    }

    if (ds3231_read_buf(rtc, DS3231_REG_SECONDS, d, 7u) != 0u)
    {
        return DS3231_STATUS_I2C_ERROR;
    }

    dt->second = ds3231_bcd_to_bin((uint8_t)(d[0] & 0x7Fu));
    dt->minute = ds3231_bcd_to_bin((uint8_t)(d[1] & 0x7Fu));
    dt->hour = ds3231_bcd_to_bin((uint8_t)(d[2] & 0x3Fu));
    dt->weekday = ds3231_bcd_to_bin((uint8_t)(d[3] & 0x07u));
    dt->day = ds3231_bcd_to_bin((uint8_t)(d[4] & 0x3Fu));
    dt->month = ds3231_bcd_to_bin((uint8_t)(d[5] & 0x1Fu));
    year2 = ds3231_bcd_to_bin(d[6]);

    /* Use 2000-based year for DS3231 century range. */
    dt->year = (uint16_t)(2000u + year2);

    if (rtc_is_valid(dt) == 0u)
    {
        return DS3231_STATUS_INVALID_DATETIME;
    }

    return DS3231_STATUS_OK;
}

int16_t ds3231_get_temperature(ds3231_t* rtc)
{
    uint8_t msb;
    uint8_t lsb;
    int16_t temp_q4;

    if ((rtc == (ds3231_t*)0) || (rtc->initialized == 0u))
    {
        return 0;
    }

    if (ds3231_read_u8(rtc, DS3231_REG_TEMP_MSB, &msb) != 0u)
    {
        return 0;
    }
    if (ds3231_read_u8(rtc, DS3231_REG_TEMP_LSB, &lsb) != 0u)
    {
        return 0;
    }

    /*
     * Temp format: 10-bit signed, MSB + LSB[7:6] fractional (0.25C steps).
     * Convert to x10 with integer math: q4 * 10 / 4.
     */
    temp_q4 = (int16_t)(((int16_t)((int8_t)msb) << 2u) | (int16_t)((lsb >> 6u) & 0x03u));
    return (int16_t)((temp_q4 * 10) / 4);
}

const rtc_driver_t* ds3231_get_driver(void)
{
    return &g_ds3231_driver;
}
