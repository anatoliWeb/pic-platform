#include "core/compiler.h"
#include "core/debug.h"
#include "drivers/communication/i2c/i2c.h"
#include "libraries/time/rtc_framework/rtc_framework.h"
#include "libraries/time/ds3231/ds3231.h"

void main(void)
{
    ds3231_t chip;
    rtc_t rtc;
    rtc_datetime_t dt;
    int16_t t_x10;
    uint32_t unix_time;

    i2c_init(100000u);

    if (ds3231_init(&chip, 0x68u) == 0u)
    {
        DBG_PRINTLN("DS3231 init failed");
        while (1)
        {
        }
    }

    /* Bind DS3231 to generic RTC framework API. */
    (void)rtc_init(&rtc, ds3231_get_driver(), &chip);

    dt.year = 2026u;
    dt.month = 1u;
    dt.day = 1u;
    dt.hour = 12u;
    dt.minute = 0u;
    dt.second = 0u;
    dt.weekday = 4u;
    (void)rtc_set_datetime(&rtc, &dt);

    while (1)
    {
        if (rtc_get_datetime(&rtc, &dt) != 0u)
        {
            DBG_PRINT("RTC: ");
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

        t_x10 = ds3231_get_temperature(&chip);
        DBG_PRINT("Temp x10: ");
        DBG_PRINT_INT((int32_t)t_x10);
        DBG_PRINTLN("");
    }
}
