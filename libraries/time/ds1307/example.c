/*
 * File: libraries/time/ds1307/example.c
 */

#include "core/compiler.h"
#include "core/debug.h"
#include "drivers/communication/i2c/i2c.h"
#include "libraries/time/rtc_framework/rtc_framework.h"
#include "libraries/time/ds1307/ds1307.h"

void main(void)
{
    ds1307_t chip;
    rtc_t rtc;
    rtc_datetime_t dt;
    uint8_t ram_value;
    uint32_t unix_time;
    ds1307_status_t st;

    i2c_init(100000u);

    if (ds1307_init(&chip, 0x68u) == 0u)
    {
        DBG_PRINTLN("DS1307 init failed");
        while (1)
        {
        }
    }

    (void)rtc_init(&rtc, ds1307_get_driver(), &chip);

    dt.year = 2026u;
    dt.month = 1u;
    dt.day = 1u;
    dt.hour = 8u;
    dt.minute = 0u;
    dt.second = 0u;
    dt.weekday = 4u;
    (void)rtc_set_datetime(&rtc, &dt);

    /* Optional RAM usage demo. */
    (void)ds1307_set_ram(&chip, 0u, 0x42u);
    if (ds1307_get_ram(&chip, 0u, &ram_value) == DS1307_STATUS_OK)
    {
        DBG_PRINT("RAM[0]=0x");
        DBG_PRINT_HEX(ram_value);
        DBG_PRINTLN("");
    }

    while (1)
    {
        st = ds1307_get_datetime(&chip, &dt);
        if (st == DS1307_STATUS_OK)
        {
            DBG_PRINT("DS1307: ");
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
        else if (st == DS1307_STATUS_OSCILLATOR_HALTED)
        {
            DBG_PRINTLN("DS1307: CH bit set (clock halted)");
        }
        else
        {
            DBG_PRINT("DS1307 read error: ");
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
