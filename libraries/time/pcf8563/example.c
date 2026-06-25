/*
 * File: libraries/time/pcf8563/example.c
 */

#include "core/compiler.h"
#include "core/debug.h"
#include "drivers/communication/i2c/i2c.h"
#include "libraries/time/rtc_framework/rtc_framework.h"
#include "libraries/time/pcf8563/pcf8563.h"

void main(void)
{
    pcf8563_t chip;
    rtc_t rtc;
    rtc_datetime_t dt;
    pcf8563_status_t st;
    uint32_t unix_time;

    i2c_init(100000u);

    if (pcf8563_init(&chip, 0x51u) == 0u)
    {
        DBG_PRINTLN("PCF8563 init failed");
        while (1)
        {
        }
    }

    /* Framework integration through backend callbacks. */
    (void)rtc_init(&rtc, pcf8563_get_driver(), &chip);

    dt.year = 2026u;
    dt.month = 1u;
    dt.day = 1u;
    dt.hour = 12u;
    dt.minute = 34u;
    dt.second = 56u;
    dt.weekday = 4u;
    (void)rtc_set_datetime(&rtc, &dt);

    /* Optional alarm demo: minute match only. */
    (void)pcf8563_set_alarm(&chip, 35, -1, -1, -1);

    while (1)
    {
        st = pcf8563_get_datetime(&chip, &dt);
        if (st == PCF8563_STATUS_OK)
        {
            DBG_PRINT("PCF8563: ");
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
        else if (st == PCF8563_STATUS_VOLTAGE_LOW)
        {
            DBG_PRINTLN("PCF8563: voltage low flag set");
        }
        else
        {
            DBG_PRINT("PCF8563 read error: ");
            DBG_PRINT_INT((int32_t)st);
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
