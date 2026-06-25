/*
 * File: libraries/time/rtc_framework/rtc_framework.c
 */

#include "libraries/time/rtc_framework/rtc_framework.h"

static uint8_t rtc_is_leap_year(uint16_t year)
{
    if ((year % 400u) == 0u)
    {
        return 1u;
    }
    if ((year % 100u) == 0u)
    {
        return 0u;
    }
    if ((year % 4u) == 0u)
    {
        return 1u;
    }
    return 0u;
}

static uint8_t rtc_days_in_month(uint16_t year, uint8_t month)
{
    static const uint8_t g_days[12] =
    {
        31u, 28u, 31u, 30u, 31u, 30u, 31u, 31u, 30u, 31u, 30u, 31u
    };

    if ((month < 1u) || (month > 12u))
    {
        return 0u;
    }

    if ((month == 2u) && (rtc_is_leap_year(year) != 0u))
    {
        return 29u;
    }

    return g_days[month - 1u];
}

static uint32_t rtc_datetime_to_unix(const rtc_datetime_t* dt)
{
    uint32_t days;
    uint16_t y;
    uint8_t m;

    days = 0u;

    for (y = 1970u; y < dt->year; y++)
    {
        days += (rtc_is_leap_year(y) != 0u) ? 366u : 365u;
    }

    for (m = 1u; m < dt->month; m++)
    {
        days += rtc_days_in_month(dt->year, m);
    }

    days += (uint32_t)(dt->day - 1u);

    return (uint32_t)(days * 86400u) +
           (uint32_t)(dt->hour * 3600u) +
           (uint32_t)(dt->minute * 60u) +
           (uint32_t)dt->second;
}

static void rtc_unix_to_datetime(uint32_t unix_time, rtc_datetime_t* dt)
{
    uint32_t days;
    uint32_t seconds_of_day;
    uint16_t year;
    uint8_t month;
    uint8_t dim;
    uint32_t ydays;

    days = unix_time / 86400u;
    seconds_of_day = unix_time % 86400u;

    year = 1970u;
    while (1)
    {
        ydays = (rtc_is_leap_year(year) != 0u) ? 366u : 365u;
        if (days < ydays)
        {
            break;
        }
        days -= ydays;
        year++;
    }

    month = 1u;
    while (1)
    {
        dim = rtc_days_in_month(year, month);
        if (days < dim)
        {
            break;
        }
        days -= dim;
        month++;
    }

    dt->year = year;
    dt->month = month;
    dt->day = (uint8_t)(days + 1u);

    dt->hour = (uint8_t)(seconds_of_day / 3600u);
    seconds_of_day %= 3600u;
    dt->minute = (uint8_t)(seconds_of_day / 60u);
    dt->second = (uint8_t)(seconds_of_day % 60u);

    /* 1970-01-01 was Thursday. We map 1..7. */
    dt->weekday = (uint8_t)(((unix_time / 86400u) + 4u) % 7u + 1u);
}

uint8_t rtc_is_valid(const rtc_datetime_t* dt)
{
    uint8_t dim;

    if (dt == (const rtc_datetime_t*)0)
    {
        return 0u;
    }

    if ((dt->year < 1970u) || (dt->year > 2099u))
    {
        return 0u;
    }

    if ((dt->month < 1u) || (dt->month > 12u))
    {
        return 0u;
    }

    dim = rtc_days_in_month(dt->year, dt->month);
    if ((dt->day < 1u) || (dt->day > dim))
    {
        return 0u;
    }

    if (dt->hour > 23u) { return 0u; }
    if (dt->minute > 59u) { return 0u; }
    if (dt->second > 59u) { return 0u; }

    return 1u;
}

uint8_t rtc_init(rtc_t* rtc, const rtc_driver_t* driver, void* driver_ctx)
{
    if (rtc == (rtc_t*)0)
    {
        return 0u;
    }

    rtc->driver_ctx = driver_ctx;
    rtc->soft_unix = 0u;
    rtc->soft_dt.year = 1970u;
    rtc->soft_dt.month = 1u;
    rtc->soft_dt.day = 1u;
    rtc->soft_dt.hour = 0u;
    rtc->soft_dt.minute = 0u;
    rtc->soft_dt.second = 0u;
    rtc->soft_dt.weekday = 4u;

    if ((driver == (const rtc_driver_t*)0) ||
        (driver->set_datetime == (uint8_t (*)(void*, const rtc_datetime_t*))0) ||
        (driver->get_datetime == (uint8_t (*)(void*, rtc_datetime_t*))0))
    {
        rtc->use_soft_rtc = 1u;
        rtc->driver.set_datetime = (uint8_t (*)(void*, const rtc_datetime_t*))0;
        rtc->driver.get_datetime = (uint8_t (*)(void*, rtc_datetime_t*))0;
    }
    else
    {
        rtc->use_soft_rtc = 0u;
        rtc->driver = *driver;
    }

    rtc->initialized = 1u;
    return 1u;
}

uint8_t rtc_set_datetime(rtc_t* rtc, const rtc_datetime_t* dt)
{
    if ((rtc == (rtc_t*)0) || (dt == (const rtc_datetime_t*)0) || (rtc->initialized == 0u))
    {
        return 0u;
    }

    if (rtc_is_valid(dt) == 0u)
    {
        return 0u;
    }

    if (rtc->use_soft_rtc != 0u)
    {
        rtc->soft_dt = *dt;
        rtc->soft_unix = rtc_datetime_to_unix(dt);
        return 1u;
    }

    return rtc->driver.set_datetime(rtc->driver_ctx, dt);
}

uint8_t rtc_get_datetime(rtc_t* rtc, rtc_datetime_t* dt)
{
    if ((rtc == (rtc_t*)0) || (dt == (rtc_datetime_t*)0) || (rtc->initialized == 0u))
    {
        return 0u;
    }

    if (rtc->use_soft_rtc != 0u)
    {
        *dt = rtc->soft_dt;
        return 1u;
    }

    return rtc->driver.get_datetime(rtc->driver_ctx, dt);
}

uint8_t rtc_set_unix(rtc_t* rtc, uint32_t unix_time)
{
    rtc_datetime_t dt;

    if ((rtc == (rtc_t*)0) || (rtc->initialized == 0u))
    {
        return 0u;
    }

    rtc_unix_to_datetime(unix_time, &dt);
    return rtc_set_datetime(rtc, &dt);
}

uint8_t rtc_get_unix(rtc_t* rtc, uint32_t* unix_time)
{
    rtc_datetime_t dt;

    if ((rtc == (rtc_t*)0) || (unix_time == (uint32_t*)0) || (rtc->initialized == 0u))
    {
        return 0u;
    }

    if (rtc_get_datetime(rtc, &dt) == 0u)
    {
        return 0u;
    }

    *unix_time = rtc_datetime_to_unix(&dt);
    return 1u;
}

void rtc_tick(rtc_t* rtc)
{
    rtc_datetime_t dt;

    if ((rtc == (rtc_t*)0) || (rtc->initialized == 0u) || (rtc->use_soft_rtc == 0u))
    {
        return;
    }

    rtc->soft_unix++;
    rtc_unix_to_datetime(rtc->soft_unix, &dt);
    rtc->soft_dt = dt;
}
