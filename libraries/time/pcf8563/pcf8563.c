/*
 * File: libraries/time/pcf8563/pcf8563.c
 */

#include "libraries/time/pcf8563/pcf8563.h"

#include "drivers/communication/i2c/i2c.h"

#define PCF8563_REG_CTRL1      0x00u
#define PCF8563_REG_CTRL2      0x01u
#define PCF8563_REG_SECONDS    0x02u
#define PCF8563_REG_MINUTES    0x03u
#define PCF8563_REG_HOURS      0x04u
#define PCF8563_REG_DAYS       0x05u
#define PCF8563_REG_WEEKDAYS   0x06u
#define PCF8563_REG_MONTHS     0x07u
#define PCF8563_REG_YEARS      0x08u
#define PCF8563_REG_ALARM_MIN  0x09u
#define PCF8563_REG_ALARM_HOUR 0x0Au
#define PCF8563_REG_ALARM_DAY  0x0Bu
#define PCF8563_REG_ALARM_WDAY 0x0Cu

#define PCF8563_VL_MASK        0x80u
#define PCF8563_AE_MASK        0x80u

static uint8_t pcf8563_bin_to_bcd(uint8_t value)
{
    return (uint8_t)(((value / 10u) << 4u) | (value % 10u));
}

static uint8_t pcf8563_bcd_to_bin(uint8_t value)
{
    return (uint8_t)(((value >> 4u) * 10u) + (value & 0x0Fu));
}

static uint8_t pcf8563_read_u8(pcf8563_t* rtc, uint8_t reg, uint8_t* value)
{
    return i2c_read_register(rtc->i2c_address, reg, value);
}

static uint8_t pcf8563_write_u8(pcf8563_t* rtc, uint8_t reg, uint8_t value)
{
    return i2c_write_register(rtc->i2c_address, reg, value);
}

static uint8_t pcf8563_read_buf(pcf8563_t* rtc, uint8_t reg, uint8_t* buf, uint8_t len)
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

static uint8_t pcf8563_set_datetime_backend(void* ctx, const rtc_datetime_t* dt)
{
    return (uint8_t)(pcf8563_set_datetime((pcf8563_t*)ctx, dt) == PCF8563_STATUS_OK ? 1u : 0u);
}

static uint8_t pcf8563_get_datetime_backend(void* ctx, rtc_datetime_t* dt)
{
    return (uint8_t)(pcf8563_get_datetime((pcf8563_t*)ctx, dt) == PCF8563_STATUS_OK ? 1u : 0u);
}

static const rtc_driver_t g_pcf8563_driver =
{
    pcf8563_set_datetime_backend,
    pcf8563_get_datetime_backend
};

uint8_t pcf8563_init(pcf8563_t* rtc, uint8_t i2c_address)
{
    uint8_t ctrl1;

    if (rtc == (pcf8563_t*)0)
    {
        return 0u;
    }

    rtc->i2c_address = i2c_address;
    rtc->initialized = 0u;

    if (pcf8563_read_u8(rtc, PCF8563_REG_CTRL1, &ctrl1) != 0u)
    {
        return 0u;
    }

    rtc->initialized = 1u;
    return 1u;
}

pcf8563_status_t pcf8563_set_datetime(pcf8563_t* rtc, const rtc_datetime_t* dt)
{
    uint8_t d[7];

    if ((rtc == (pcf8563_t*)0) || (dt == (const rtc_datetime_t*)0) || (rtc->initialized == 0u))
    {
        return PCF8563_STATUS_ERROR;
    }

    if (rtc_is_valid(dt) == 0u)
    {
        return PCF8563_STATUS_INVALID_DATETIME;
    }

    d[0] = (uint8_t)(pcf8563_bin_to_bcd(dt->second) & 0x7Fu);
    d[1] = (uint8_t)(pcf8563_bin_to_bcd(dt->minute) & 0x7Fu);
    d[2] = (uint8_t)(pcf8563_bin_to_bcd(dt->hour) & 0x3Fu);
    d[3] = (uint8_t)(pcf8563_bin_to_bcd(dt->day) & 0x3Fu);
    d[4] = (uint8_t)(pcf8563_bin_to_bcd((uint8_t)((dt->weekday < 1u || dt->weekday > 7u) ? 1u : dt->weekday)) & 0x07u);
    d[5] = (uint8_t)(pcf8563_bin_to_bcd(dt->month) & 0x1Fu);
    d[6] = pcf8563_bin_to_bcd((uint8_t)(dt->year % 100u));

    if (pcf8563_write_u8(rtc, PCF8563_REG_SECONDS, d[0]) != 0u) { return PCF8563_STATUS_I2C_ERROR; }
    if (pcf8563_write_u8(rtc, PCF8563_REG_MINUTES, d[1]) != 0u) { return PCF8563_STATUS_I2C_ERROR; }
    if (pcf8563_write_u8(rtc, PCF8563_REG_HOURS, d[2]) != 0u) { return PCF8563_STATUS_I2C_ERROR; }
    if (pcf8563_write_u8(rtc, PCF8563_REG_DAYS, d[3]) != 0u) { return PCF8563_STATUS_I2C_ERROR; }
    if (pcf8563_write_u8(rtc, PCF8563_REG_WEEKDAYS, d[4]) != 0u) { return PCF8563_STATUS_I2C_ERROR; }
    if (pcf8563_write_u8(rtc, PCF8563_REG_MONTHS, d[5]) != 0u) { return PCF8563_STATUS_I2C_ERROR; }
    if (pcf8563_write_u8(rtc, PCF8563_REG_YEARS, d[6]) != 0u) { return PCF8563_STATUS_I2C_ERROR; }

    return PCF8563_STATUS_OK;
}

pcf8563_status_t pcf8563_get_datetime(pcf8563_t* rtc, rtc_datetime_t* dt)
{
    uint8_t d[7];

    if ((rtc == (pcf8563_t*)0) || (dt == (rtc_datetime_t*)0) || (rtc->initialized == 0u))
    {
        return PCF8563_STATUS_ERROR;
    }

    if (pcf8563_read_buf(rtc, PCF8563_REG_SECONDS, d, 7u) != 0u)
    {
        return PCF8563_STATUS_I2C_ERROR;
    }

    if ((d[0] & PCF8563_VL_MASK) != 0u)
    {
        return PCF8563_STATUS_VOLTAGE_LOW;
    }

    dt->second = pcf8563_bcd_to_bin((uint8_t)(d[0] & 0x7Fu));
    dt->minute = pcf8563_bcd_to_bin((uint8_t)(d[1] & 0x7Fu));
    dt->hour = pcf8563_bcd_to_bin((uint8_t)(d[2] & 0x3Fu));
    dt->day = pcf8563_bcd_to_bin((uint8_t)(d[3] & 0x3Fu));
    dt->weekday = pcf8563_bcd_to_bin((uint8_t)(d[4] & 0x07u));
    dt->month = pcf8563_bcd_to_bin((uint8_t)(d[5] & 0x1Fu));
    dt->year = (uint16_t)(2000u + pcf8563_bcd_to_bin(d[6]));

    if (rtc_is_valid(dt) == 0u)
    {
        return PCF8563_STATUS_INVALID_DATETIME;
    }

    return PCF8563_STATUS_OK;
}

pcf8563_status_t pcf8563_set_alarm(pcf8563_t* rtc,
                                   int8_t minute,
                                   int8_t hour,
                                   int8_t day,
                                   int8_t weekday)
{
    uint8_t m;
    uint8_t h;
    uint8_t d;
    uint8_t w;

    if ((rtc == (pcf8563_t*)0) || (rtc->initialized == 0u))
    {
        return PCF8563_STATUS_ERROR;
    }

    m = (minute < 0) ? PCF8563_AE_MASK : (uint8_t)(pcf8563_bin_to_bcd((uint8_t)minute) & 0x7Fu);
    h = (hour < 0) ? PCF8563_AE_MASK : (uint8_t)(pcf8563_bin_to_bcd((uint8_t)hour) & 0x3Fu);
    d = (day < 0) ? PCF8563_AE_MASK : (uint8_t)(pcf8563_bin_to_bcd((uint8_t)day) & 0x3Fu);
    w = (weekday < 0) ? PCF8563_AE_MASK : (uint8_t)(pcf8563_bin_to_bcd((uint8_t)weekday) & 0x07u);

    if (pcf8563_write_u8(rtc, PCF8563_REG_ALARM_MIN, m) != 0u) { return PCF8563_STATUS_I2C_ERROR; }
    if (pcf8563_write_u8(rtc, PCF8563_REG_ALARM_HOUR, h) != 0u) { return PCF8563_STATUS_I2C_ERROR; }
    if (pcf8563_write_u8(rtc, PCF8563_REG_ALARM_DAY, d) != 0u) { return PCF8563_STATUS_I2C_ERROR; }
    if (pcf8563_write_u8(rtc, PCF8563_REG_ALARM_WDAY, w) != 0u) { return PCF8563_STATUS_I2C_ERROR; }

    return PCF8563_STATUS_OK;
}

pcf8563_status_t pcf8563_clear_alarm(pcf8563_t* rtc)
{
    return pcf8563_set_alarm(rtc, -1, -1, -1, -1);
}

const rtc_driver_t* pcf8563_get_driver(void)
{
    return &g_pcf8563_driver;
}
