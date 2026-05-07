#include "core/compiler.h"
#include "core/debug.h"
#include "libraries/time/rtc_framework/rtc_framework.h"

void main(void)
{
    rtc_t rtc;
    rtc_datetime_t dt;
    uint32_t unix_time;

    /* Initialize without hardware backend: software RTC mode. */
    (void)rtc_init(&rtc, (const rtc_driver_t*)0, (void*)0);

    dt.year = 2026u;
    dt.month = 1u;
    dt.day = 1u;
    dt.hour = 12u;
    dt.minute = 0u;
    dt.second = 0u;
    dt.weekday = 4u;
    (void)rtc_set_datetime(&rtc, &dt);

    if (rtc_get_unix(&rtc, &unix_time) != 0u)
    {
        DBG_PRINT("UNIX: ");
        DBG_PRINT_INT((int32_t)unix_time);
        DBG_PRINTLN("");
    }

    while (1)
    {
        /* Simulated 1-second tick (normally called from timer ISR/task). */
        rtc_tick(&rtc);

        if (rtc_get_datetime(&rtc, &dt) != 0u)
        {
            DBG_PRINT("Date: ");
            DBG_PRINT_INT((int32_t)dt.year);
            DBG_PRINT("-");
            DBG_PRINT_INT((int32_t)dt.month);
            DBG_PRINT("-");
            DBG_PRINT_INT((int32_t)dt.day);
            DBG_PRINT(" Time: ");
            DBG_PRINT_INT((int32_t)dt.hour);
            DBG_PRINT(":");
            DBG_PRINT_INT((int32_t)dt.minute);
            DBG_PRINT(":");
            DBG_PRINT_INT((int32_t)dt.second);
            DBG_PRINTLN("");
        }
    }
}
