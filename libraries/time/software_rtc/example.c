#include "core/compiler.h"
#include "core/debug.h"
#include "libraries/time/rtc_framework/rtc_framework.h"
#include "libraries/time/software_rtc/software_rtc.h"

void main(void)
{
    software_rtc_t sw;
    rtc_t rtc;
    rtc_datetime_t dt;
    uint32_t unix_time;
    uint16_t ms_step;
    uint16_t print_accum_ms;

    dt.year = 2026u;
    dt.month = 1u;
    dt.day = 1u;
    dt.hour = 0u;
    dt.minute = 0u;
    dt.second = 0u;
    dt.weekday = 4u;

    (void)software_rtc_init(&sw, &dt);
    (void)rtc_init(&rtc, software_rtc_get_driver(), &sw);

    ms_step = 100u;
    print_accum_ms = 0u;
    while (1)
    {
        /* Simulated periodic tick from scheduler/ISR. */
        software_rtc_tick_ms(&sw, ms_step);
        print_accum_ms = (uint16_t)(print_accum_ms + ms_step);

        if (print_accum_ms >= 1000u)
        {
            print_accum_ms = 0u;

            if (rtc_get_datetime(&rtc, &dt) != 0u)
            {
                DBG_PRINT("SW RTC: ");
                DBG_PRINT_INT((int32_t)dt.year);
                DBG_PRINT("-");
                DBG_PRINT_INT((int32_t)dt.month);
                DBG_PRINT("-");
                DBG_PRINT_INT((int32_t)dt.day);
                DBG_PRINT(" ");
                DBG_PRINT_INT((int32_t)dt.hour);
                DBG_PRINT(":");
                DBG_PRINT_INT((int32_t)dt.minute);
                DBG_PRINT(":");
                DBG_PRINT_INT((int32_t)dt.second);
                DBG_PRINTLN("");
            }

            if (rtc_get_unix(&rtc, &unix_time) != 0u)
            {
                DBG_PRINT("UNIX: ");
                DBG_PRINT_INT((int32_t)unix_time);
                DBG_PRINTLN("");
            }
        }
    }
}
