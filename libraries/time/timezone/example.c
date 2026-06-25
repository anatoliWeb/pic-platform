/*
 * File: libraries/time/timezone/example.c
 */

#include "core/compiler.h"
#include "core/debug.h"
#include "libraries/time/timezone/timezone.h"

static void print_dt(const char* label, const rtc_datetime_t* dt)
{
    DBG_PRINT(label);
    DBG_PRINT(" ");
    DBG_PRINT_INT((int32_t)dt->year);
    DBG_PRINT("-");
    DBG_PRINT_INT((int32_t)dt->month);
    DBG_PRINT("-");
    DBG_PRINT_INT((int32_t)dt->day);
    DBG_PRINT(" ");
    DBG_PRINT_INT((int32_t)dt->hour);
    DBG_PRINT(":");
    DBG_PRINT_INT((int32_t)dt->minute);
    DBG_PRINT(":");
    DBG_PRINT_INT((int32_t)dt->second);
    DBG_PRINTLN("");
}

void main(void)
{
    timezone_t tz;
    rtc_datetime_t utc_dt;
    rtc_datetime_t local_dt;
    rtc_datetime_t back_utc;

    (void)timezone_init(&tz);
    timezone_set_offset_minutes(&tz, 120); /* UTC+2 */
    timezone_set_dst(&tz, 0u);

    utc_dt.year = 2026u;
    utc_dt.month = 3u;
    utc_dt.day = 28u;
    utc_dt.hour = 22u;
    utc_dt.minute = 30u;
    utc_dt.second = 0u;
    utc_dt.weekday = 6u;

    if (timezone_utc_to_local(&tz, &utc_dt, &local_dt) != 0u)
    {
        print_dt("UTC   :", &utc_dt);
        print_dt("Local :", &local_dt);
    }

    timezone_set_dst(&tz, 1u); /* +1 hour */
    if (timezone_utc_to_local(&tz, &utc_dt, &local_dt) != 0u)
    {
        print_dt("Local DST:", &local_dt);
    }

    if (timezone_local_to_utc(&tz, &local_dt, &back_utc) != 0u)
    {
        print_dt("Back UTC :", &back_utc);
    }

    while (1)
    {
    }
}
