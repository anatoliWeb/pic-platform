#include "libraries/time/software_rtc/software_rtc.h"

static uint8_t software_rtc_is_leap_year(uint16_t year)
{
    if ((year % 400u) == 0u) { return 1u; }
    if ((year % 100u) == 0u) { return 0u; }
    if ((year % 4u) == 0u) { return 1u; }
    return 0u;
}

static uint8_t software_rtc_days_in_month(uint16_t year, uint8_t month)
{
    static const uint8_t g_days[12] =
    {
        31u, 28u, 31u, 30u, 31u, 30u, 31u, 31u, 30u, 31u, 30u, 31u
    };

    if ((month < 1u) || (month > 12u))
    {
        return 0u;
    }

    if ((month == 2u) && (software_rtc_is_leap_year(year) != 0u))
    {
        return 29u;
    }

    return g_days[month - 1u];
}

static void software_rtc_step_one_second(software_rtc_t* sw)
{
    uint8_t dim;

    sw->datetime.second++;
    if (sw->datetime.second < 60u)
    {
        return;
    }
    sw->datetime.second = 0u;

    sw->datetime.minute++;
    if (sw->datetime.minute < 60u)
    {
        return;
    }
    sw->datetime.minute = 0u;

    sw->datetime.hour++;
    if (sw->datetime.hour < 24u)
    {
        return;
    }
    sw->datetime.hour = 0u;

    /* Advance weekday as 1..7. */
    if ((sw->datetime.weekday < 1u) || (sw->datetime.weekday > 7u))
    {
        sw->datetime.weekday = 1u;
    }
    else
    {
        sw->datetime.weekday = (uint8_t)((sw->datetime.weekday % 7u) + 1u);
    }

    sw->datetime.day++;
    dim = software_rtc_days_in_month(sw->datetime.year, sw->datetime.month);
    if (sw->datetime.day <= dim)
    {
        return;
    }
    sw->datetime.day = 1u;

    sw->datetime.month++;
    if (sw->datetime.month <= 12u)
    {
        return;
    }
    sw->datetime.month = 1u;
    sw->datetime.year++;
}

static uint8_t software_rtc_set_datetime_impl(void* ctx, const rtc_datetime_t* dt)
{
    return software_rtc_set_datetime((software_rtc_t*)ctx, dt);
}

static uint8_t software_rtc_get_datetime_impl(void* ctx, rtc_datetime_t* dt)
{
    return software_rtc_get_datetime((software_rtc_t*)ctx, dt);
}

static const rtc_driver_t g_software_rtc_driver =
{
    software_rtc_set_datetime_impl,
    software_rtc_get_datetime_impl
};

uint8_t software_rtc_init(software_rtc_t* sw, const rtc_datetime_t* initial_dt)
{
    rtc_datetime_t dt;

    if (sw == (software_rtc_t*)0)
    {
        return 0u;
    }

    if (initial_dt == (const rtc_datetime_t*)0)
    {
        dt.year = 1970u;
        dt.month = 1u;
        dt.day = 1u;
        dt.hour = 0u;
        dt.minute = 0u;
        dt.second = 0u;
        dt.weekday = 4u;
        initial_dt = &dt;
    }

    if (rtc_is_valid(initial_dt) == 0u)
    {
        return 0u;
    }

    sw->datetime = *initial_dt;
    sw->ms_accum = 0u;
    sw->running = 1u;
    return 1u;
}

void software_rtc_tick_ms(software_rtc_t* sw, uint16_t elapsed_ms)
{
    uint32_t total_ms;
    uint32_t seconds;

    if ((sw == (software_rtc_t*)0) || (sw->running == 0u))
    {
        return;
    }

    total_ms = (uint32_t)sw->ms_accum + (uint32_t)elapsed_ms;
    if (total_ms < 1000u)
    {
        sw->ms_accum = (uint16_t)total_ms;
        return;
    }

    seconds = total_ms / 1000u;
    sw->ms_accum = (uint16_t)(total_ms % 1000u);
    software_rtc_tick_seconds(sw, seconds);
}

void software_rtc_tick_seconds(software_rtc_t* sw, uint32_t elapsed_seconds)
{
    uint32_t i;

    if ((sw == (software_rtc_t*)0) || (sw->running == 0u))
    {
        return;
    }

    for (i = 0u; i < elapsed_seconds; i++)
    {
        software_rtc_step_one_second(sw);
    }
}

uint8_t software_rtc_set_datetime(software_rtc_t* sw, const rtc_datetime_t* dt)
{
    if ((sw == (software_rtc_t*)0) || (dt == (const rtc_datetime_t*)0))
    {
        return 0u;
    }

    if (rtc_is_valid(dt) == 0u)
    {
        return 0u;
    }

    sw->datetime = *dt;
    return 1u;
}

uint8_t software_rtc_get_datetime(software_rtc_t* sw, rtc_datetime_t* dt)
{
    if ((sw == (software_rtc_t*)0) || (dt == (rtc_datetime_t*)0))
    {
        return 0u;
    }

    *dt = sw->datetime;
    return 1u;
}

void software_rtc_sync(software_rtc_t* sw, const rtc_datetime_t* dt)
{
    if ((sw == (software_rtc_t*)0) || (dt == (const rtc_datetime_t*)0))
    {
        return;
    }

    if (rtc_is_valid(dt) == 0u)
    {
        return;
    }

    sw->datetime = *dt;
    sw->ms_accum = 0u;
}

uint8_t software_rtc_is_running(const software_rtc_t* sw)
{
    if (sw == (const software_rtc_t*)0)
    {
        return 0u;
    }
    return sw->running;
}

const rtc_driver_t* software_rtc_get_driver(void)
{
    return &g_software_rtc_driver;
}
