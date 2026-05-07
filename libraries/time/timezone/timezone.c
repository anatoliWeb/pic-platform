#include "libraries/time/timezone/timezone.h"

static uint8_t timezone_is_leap_year(uint16_t year)
{
    if ((year % 400u) == 0u) { return 1u; }
    if ((year % 100u) == 0u) { return 0u; }
    if ((year % 4u) == 0u) { return 1u; }
    return 0u;
}

static uint8_t timezone_days_in_month(uint16_t year, uint8_t month)
{
    static const uint8_t g_days[12] =
    {
        31u, 28u, 31u, 30u, 31u, 30u, 31u, 31u, 30u, 31u, 30u, 31u
    };

    if ((month < 1u) || (month > 12u))
    {
        return 0u;
    }

    if ((month == 2u) && (timezone_is_leap_year(year) != 0u))
    {
        return 29u;
    }

    return g_days[month - 1u];
}

static uint32_t timezone_datetime_to_unix(const rtc_datetime_t* dt)
{
    uint32_t days;
    uint16_t y;
    uint8_t m;

    days = 0u;
    for (y = 1970u; y < dt->year; y++)
    {
        days += (timezone_is_leap_year(y) != 0u) ? 366u : 365u;
    }

    for (m = 1u; m < dt->month; m++)
    {
        days += timezone_days_in_month(dt->year, m);
    }

    days += (uint32_t)(dt->day - 1u);

    return (uint32_t)(days * 86400u) +
           (uint32_t)(dt->hour * 3600u) +
           (uint32_t)(dt->minute * 60u) +
           (uint32_t)dt->second;
}

static void timezone_unix_to_datetime(uint32_t unix_time, rtc_datetime_t* dt)
{
    uint32_t days;
    uint32_t sod;
    uint16_t year;
    uint8_t month;
    uint8_t dim;
    uint32_t ydays;

    days = unix_time / 86400u;
    sod = unix_time % 86400u;

    year = 1970u;
    while (1)
    {
        ydays = (timezone_is_leap_year(year) != 0u) ? 366u : 365u;
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
        dim = timezone_days_in_month(year, month);
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
    dt->hour = (uint8_t)(sod / 3600u);
    sod %= 3600u;
    dt->minute = (uint8_t)(sod / 60u);
    dt->second = (uint8_t)(sod % 60u);
    dt->weekday = (uint8_t)(((unix_time / 86400u) + 4u) % 7u + 1u);
}

static int32_t timezone_total_offset_seconds(const timezone_t* tz)
{
    int32_t sec;

    sec = (int32_t)tz->offset_minutes * 60;
    if (tz->dst_enabled != 0u)
    {
        sec += 3600;
    }
    return sec;
}

uint8_t timezone_init(timezone_t* tz)
{
    if (tz == (timezone_t*)0)
    {
        return 0u;
    }

    tz->offset_minutes = 0;
    tz->dst_enabled = 0u;
    tz->initialized = 1u;
    return 1u;
}

void timezone_set_offset_minutes(timezone_t* tz, int16_t offset_minutes)
{
    if ((tz == (timezone_t*)0) || (tz->initialized == 0u))
    {
        return;
    }

    tz->offset_minutes = offset_minutes;
}

void timezone_set_dst(timezone_t* tz, uint8_t enabled)
{
    if ((tz == (timezone_t*)0) || (tz->initialized == 0u))
    {
        return;
    }

    tz->dst_enabled = (uint8_t)(enabled != 0u ? 1u : 0u);
}

int16_t timezone_get_offset_minutes(const timezone_t* tz)
{
    if ((tz == (const timezone_t*)0) || (tz->initialized == 0u))
    {
        return 0;
    }

    return tz->offset_minutes;
}

uint8_t timezone_is_dst_enabled(const timezone_t* tz)
{
    if ((tz == (const timezone_t*)0) || (tz->initialized == 0u))
    {
        return 0u;
    }

    return tz->dst_enabled;
}

uint8_t timezone_utc_to_local(const timezone_t* tz, const rtc_datetime_t* utc_dt, rtc_datetime_t* local_dt)
{
    uint32_t unix_utc;
    int32_t unix_local;
    int32_t off_sec;

    if ((tz == (const timezone_t*)0) || (utc_dt == (const rtc_datetime_t*)0) || (local_dt == (rtc_datetime_t*)0))
    {
        return 0u;
    }

    if ((tz->initialized == 0u) || (rtc_is_valid(utc_dt) == 0u))
    {
        return 0u;
    }

    unix_utc = timezone_datetime_to_unix(utc_dt);
    off_sec = timezone_total_offset_seconds(tz);
    unix_local = (int32_t)unix_utc + off_sec;

    if (unix_local < 0)
    {
        return 0u;
    }

    timezone_unix_to_datetime((uint32_t)unix_local, local_dt);
    return 1u;
}

uint8_t timezone_local_to_utc(const timezone_t* tz, const rtc_datetime_t* local_dt, rtc_datetime_t* utc_dt)
{
    uint32_t unix_local;
    int32_t unix_utc;
    int32_t off_sec;

    if ((tz == (const timezone_t*)0) || (local_dt == (const rtc_datetime_t*)0) || (utc_dt == (rtc_datetime_t*)0))
    {
        return 0u;
    }

    if ((tz->initialized == 0u) || (rtc_is_valid(local_dt) == 0u))
    {
        return 0u;
    }

    unix_local = timezone_datetime_to_unix(local_dt);
    off_sec = timezone_total_offset_seconds(tz);
    unix_utc = (int32_t)unix_local - off_sec;

    if (unix_utc < 0)
    {
        return 0u;
    }

    timezone_unix_to_datetime((uint32_t)unix_utc, utc_dt);
    return 1u;
}
