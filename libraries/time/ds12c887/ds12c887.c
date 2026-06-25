/*
 * File: libraries/time/ds12c887/ds12c887.c
 */

#include "libraries/time/ds12c887/ds12c887.h"

#include "core/delay.h"
#include "drivers/gpio/gpio.h"

#define DS12C887_REG_SECONDS      0x00u
#define DS12C887_REG_SECONDS_ALM  0x01u
#define DS12C887_REG_MINUTES      0x02u
#define DS12C887_REG_MINUTES_ALM  0x03u
#define DS12C887_REG_HOURS        0x04u
#define DS12C887_REG_HOURS_ALM    0x05u
#define DS12C887_REG_WEEKDAY      0x06u
#define DS12C887_REG_DAY          0x07u
#define DS12C887_REG_MONTH        0x08u
#define DS12C887_REG_YEAR         0x09u
#define DS12C887_REG_A            0x0Au
#define DS12C887_REG_B            0x0Bu
#define DS12C887_REG_C            0x0Cu
#define DS12C887_REG_D            0x0Du

#define DS12C887_REGA_UIP_MASK    0x80u

#define DS12C887_REGB_SET_MASK    0x80u
#define DS12C887_REGB_PIE_MASK    0x40u
#define DS12C887_REGB_24H_MASK    0x02u
#define DS12C887_REGB_DM_MASK     0x04u

#define DS12C887_REGD_VRT_MASK    0x80u

static uint8_t ds12c887_bin_to_bcd(uint8_t value)
{
    return (uint8_t)(((value / 10u) << 4u) | (value % 10u));
}

static uint8_t ds12c887_bcd_to_bin(uint8_t value)
{
    return (uint8_t)(((value >> 4u) * 10u) + (value & 0x0Fu));
}

static void ds12c887_ctrl_set(ds12c887_t* rtc, uint8_t pin, uint8_t high)
{
    if (high != 0u)
    {
        gpio_write_high(rtc->ctrl_port, pin);
    }
    else
    {
        gpio_write_low(rtc->ctrl_port, pin);
    }
}

static void ds12c887_ctrl_idle(ds12c887_t* rtc)
{
    /* Active-low control lines: idle all high. */
    ds12c887_ctrl_set(rtc, rtc->pin_cs, 1u);
    ds12c887_ctrl_set(rtc, rtc->pin_as, 1u);
    ds12c887_ctrl_set(rtc, rtc->pin_ds, 1u);
    ds12c887_ctrl_set(rtc, rtc->pin_rw, 1u);
}

static void ds12c887_data_output(ds12c887_t* rtc)
{
    *rtc->data_tris = 0x00u;
}

static void ds12c887_data_input(ds12c887_t* rtc)
{
    *rtc->data_tris = 0xFFu;
}

static void ds12c887_set_index(ds12c887_t* rtc, uint8_t index)
{
    *rtc->addr_port = index;
}

static void ds12c887_pulse_as(ds12c887_t* rtc)
{
    ds12c887_ctrl_set(rtc, rtc->pin_as, 0u);
    DRV_DELAY_US(1u);
    ds12c887_ctrl_set(rtc, rtc->pin_as, 1u);
    DRV_DELAY_US(1u);
}

static void ds12c887_pulse_ds(ds12c887_t* rtc)
{
    ds12c887_ctrl_set(rtc, rtc->pin_ds, 0u);
    DRV_DELAY_US(1u);
    ds12c887_ctrl_set(rtc, rtc->pin_ds, 1u);
    DRV_DELAY_US(1u);
}

uint8_t ds12c887_write_cmos(ds12c887_t* rtc, uint8_t index, uint8_t value)
{
    if ((rtc == (ds12c887_t*)0) || (rtc->initialized == 0u))
    {
        return 0u;
    }

    ds12c887_data_output(rtc);
    ds12c887_set_index(rtc, index);
    *rtc->data_port = value;

    ds12c887_ctrl_set(rtc, rtc->pin_cs, 0u);
    ds12c887_ctrl_set(rtc, rtc->pin_rw, 0u);
    ds12c887_pulse_as(rtc);
    ds12c887_pulse_ds(rtc);
    ds12c887_ctrl_idle(rtc);

    return 1u;
}

uint8_t ds12c887_read_cmos(ds12c887_t* rtc, uint8_t index, uint8_t* value)
{
    if ((rtc == (ds12c887_t*)0) || (rtc->initialized == 0u) || (value == (uint8_t*)0))
    {
        return 0u;
    }

    ds12c887_data_input(rtc);
    ds12c887_set_index(rtc, index);

    ds12c887_ctrl_set(rtc, rtc->pin_cs, 0u);
    ds12c887_ctrl_set(rtc, rtc->pin_rw, 1u);
    ds12c887_pulse_as(rtc);

    ds12c887_ctrl_set(rtc, rtc->pin_ds, 0u);
    DRV_DELAY_US(1u);
    *value = *rtc->data_port;
    ds12c887_ctrl_set(rtc, rtc->pin_ds, 1u);
    DRV_DELAY_US(1u);

    ds12c887_ctrl_idle(rtc);
    return 1u;
}

static uint8_t ds12c887_wait_uip_clear(ds12c887_t* rtc)
{
    uint16_t t;
    uint16_t timeout_us;
    uint8_t a;

    timeout_us = (rtc->uip_timeout_us == 0u) ? 2000u : rtc->uip_timeout_us;
    for (t = 0u; t < timeout_us; t++)
    {
        if (ds12c887_read_cmos(rtc, DS12C887_REG_A, &a) == 0u)
        {
            return 0u;
        }

        if ((a & DS12C887_REGA_UIP_MASK) == 0u)
        {
            return 1u;
        }

        DRV_DELAY_US(1u);
    }

    return 0u;
}

static uint8_t ds12c887_set_datetime_backend(void* ctx, const rtc_datetime_t* dt)
{
    return (uint8_t)(ds12c887_set_datetime((ds12c887_t*)ctx, dt) == DS12C887_STATUS_OK ? 1u : 0u);
}

static uint8_t ds12c887_get_datetime_backend(void* ctx, rtc_datetime_t* dt)
{
    return (uint8_t)(ds12c887_get_datetime((ds12c887_t*)ctx, dt) == DS12C887_STATUS_OK ? 1u : 0u);
}

static const rtc_driver_t g_ds12c887_driver =
{
    ds12c887_set_datetime_backend,
    ds12c887_get_datetime_backend
};

uint8_t ds12c887_init(ds12c887_t* rtc)
{
    uint8_t d;

    if (rtc == (ds12c887_t*)0)
    {
        return 0u;
    }

    *rtc->addr_tris = 0x00u;
    gpio_set_output(rtc->ctrl_tris, rtc->pin_cs);
    gpio_set_output(rtc->ctrl_tris, rtc->pin_as);
    gpio_set_output(rtc->ctrl_tris, rtc->pin_ds);
    gpio_set_output(rtc->ctrl_tris, rtc->pin_rw);
    ds12c887_ctrl_idle(rtc);

    rtc->century_reg = (rtc->century_reg == 0u) ? 0x32u : rtc->century_reg;
    rtc->uip_timeout_us = (rtc->uip_timeout_us == 0u) ? 2000u : rtc->uip_timeout_us;

    if (ds12c887_read_cmos(rtc, DS12C887_REG_D, &d) == 0u)
    {
        return 0u;
    }

    rtc->initialized = 1u;
    return 1u;
}

ds12c887_status_t ds12c887_set_datetime(ds12c887_t* rtc, const rtc_datetime_t* dt)
{
    uint8_t b;
    uint8_t century;

    if ((rtc == (ds12c887_t*)0) || (dt == (const rtc_datetime_t*)0) || (rtc->initialized == 0u))
    {
        return DS12C887_STATUS_ERROR;
    }

    if (rtc_is_valid(dt) == 0u)
    {
        return DS12C887_STATUS_INVALID_DATETIME;
    }

    if (ds12c887_read_cmos(rtc, DS12C887_REG_B, &b) == 0u)
    {
        return DS12C887_STATUS_ERROR;
    }

    /* Freeze updates while writing time. */
    b |= DS12C887_REGB_SET_MASK;
    if (ds12c887_write_cmos(rtc, DS12C887_REG_B, b) == 0u)
    {
        return DS12C887_STATUS_ERROR;
    }

    /* Force 24-hour BCD mode. */
    b &= (uint8_t)(~DS12C887_REGB_DM_MASK);
    b |= DS12C887_REGB_24H_MASK;

    if (ds12c887_write_cmos(rtc, DS12C887_REG_SECONDS, ds12c887_bin_to_bcd(dt->second)) == 0u) { return DS12C887_STATUS_ERROR; }
    if (ds12c887_write_cmos(rtc, DS12C887_REG_MINUTES, ds12c887_bin_to_bcd(dt->minute)) == 0u) { return DS12C887_STATUS_ERROR; }
    if (ds12c887_write_cmos(rtc, DS12C887_REG_HOURS, ds12c887_bin_to_bcd(dt->hour)) == 0u) { return DS12C887_STATUS_ERROR; }
    if (ds12c887_write_cmos(rtc, DS12C887_REG_WEEKDAY, ds12c887_bin_to_bcd((uint8_t)((dt->weekday < 1u || dt->weekday > 7u) ? 1u : dt->weekday))) == 0u) { return DS12C887_STATUS_ERROR; }
    if (ds12c887_write_cmos(rtc, DS12C887_REG_DAY, ds12c887_bin_to_bcd(dt->day)) == 0u) { return DS12C887_STATUS_ERROR; }
    if (ds12c887_write_cmos(rtc, DS12C887_REG_MONTH, ds12c887_bin_to_bcd(dt->month)) == 0u) { return DS12C887_STATUS_ERROR; }
    if (ds12c887_write_cmos(rtc, DS12C887_REG_YEAR, ds12c887_bin_to_bcd((uint8_t)(dt->year % 100u))) == 0u) { return DS12C887_STATUS_ERROR; }

    century = (uint8_t)(dt->year / 100u);
    (void)ds12c887_write_cmos(rtc, rtc->century_reg, ds12c887_bin_to_bcd(century));

    /* Commit configuration and unfreeze updates. */
    b &= (uint8_t)(~DS12C887_REGB_SET_MASK);
    if (ds12c887_write_cmos(rtc, DS12C887_REG_B, b) == 0u)
    {
        return DS12C887_STATUS_ERROR;
    }

    return DS12C887_STATUS_OK;
}

ds12c887_status_t ds12c887_get_datetime(ds12c887_t* rtc, rtc_datetime_t* dt)
{
    uint8_t a;
    uint8_t b;
    uint8_t sec;
    uint8_t min;
    uint8_t hr;
    uint8_t wday;
    uint8_t day;
    uint8_t mon;
    uint8_t year;
    uint8_t century;

    if ((rtc == (ds12c887_t*)0) || (dt == (rtc_datetime_t*)0) || (rtc->initialized == 0u))
    {
        return DS12C887_STATUS_ERROR;
    }

    if (ds12c887_wait_uip_clear(rtc) == 0u)
    {
        return DS12C887_STATUS_UIP_TIMEOUT;
    }

    if (ds12c887_read_cmos(rtc, DS12C887_REG_A, &a) == 0u) { return DS12C887_STATUS_ERROR; }
    if ((a & DS12C887_REGA_UIP_MASK) != 0u) { return DS12C887_STATUS_UIP_TIMEOUT; }

    if (ds12c887_read_cmos(rtc, DS12C887_REG_B, &b) == 0u) { return DS12C887_STATUS_ERROR; }
    if (ds12c887_read_cmos(rtc, DS12C887_REG_SECONDS, &sec) == 0u) { return DS12C887_STATUS_ERROR; }
    if (ds12c887_read_cmos(rtc, DS12C887_REG_MINUTES, &min) == 0u) { return DS12C887_STATUS_ERROR; }
    if (ds12c887_read_cmos(rtc, DS12C887_REG_HOURS, &hr) == 0u) { return DS12C887_STATUS_ERROR; }
    if (ds12c887_read_cmos(rtc, DS12C887_REG_WEEKDAY, &wday) == 0u) { return DS12C887_STATUS_ERROR; }
    if (ds12c887_read_cmos(rtc, DS12C887_REG_DAY, &day) == 0u) { return DS12C887_STATUS_ERROR; }
    if (ds12c887_read_cmos(rtc, DS12C887_REG_MONTH, &mon) == 0u) { return DS12C887_STATUS_ERROR; }
    if (ds12c887_read_cmos(rtc, DS12C887_REG_YEAR, &year) == 0u) { return DS12C887_STATUS_ERROR; }
    if (ds12c887_read_cmos(rtc, rtc->century_reg, &century) == 0u) { century = ds12c887_bin_to_bcd(20u); }

    if ((b & DS12C887_REGB_DM_MASK) != 0u)
    {
        /* Binary mode */
        dt->second = sec;
        dt->minute = min;
        dt->hour = (uint8_t)(hr & 0x3Fu);
        dt->weekday = wday;
        dt->day = day;
        dt->month = mon;
        dt->year = (uint16_t)(((uint16_t)century * 100u) + year);
    }
    else
    {
        /* BCD mode */
        dt->second = ds12c887_bcd_to_bin((uint8_t)(sec & 0x7Fu));
        dt->minute = ds12c887_bcd_to_bin((uint8_t)(min & 0x7Fu));
        dt->hour = ds12c887_bcd_to_bin((uint8_t)(hr & 0x3Fu));
        dt->weekday = ds12c887_bcd_to_bin((uint8_t)(wday & 0x07u));
        dt->day = ds12c887_bcd_to_bin((uint8_t)(day & 0x3Fu));
        dt->month = ds12c887_bcd_to_bin((uint8_t)(mon & 0x1Fu));
        dt->year = (uint16_t)(((uint16_t)ds12c887_bcd_to_bin(century) * 100u) + ds12c887_bcd_to_bin(year));
    }

    if (rtc_is_valid(dt) == 0u)
    {
        return DS12C887_STATUS_INVALID_DATA;
    }

    return DS12C887_STATUS_OK;
}

void ds12c887_enable_periodic_irq(ds12c887_t* rtc, uint8_t enabled)
{
    uint8_t b;

    if ((rtc == (ds12c887_t*)0) || (rtc->initialized == 0u))
    {
        return;
    }

    if (ds12c887_read_cmos(rtc, DS12C887_REG_B, &b) == 0u)
    {
        return;
    }

    if (enabled != 0u)
    {
        b |= DS12C887_REGB_PIE_MASK;
    }
    else
    {
        b &= (uint8_t)(~DS12C887_REGB_PIE_MASK);
    }

    (void)ds12c887_write_cmos(rtc, DS12C887_REG_B, b);
}

const rtc_driver_t* ds12c887_get_driver(void)
{
    return &g_ds12c887_driver;
}
