/*
 * File: libraries/time/ds1302/example.c
 */

#include "core/compiler.h"
#include "core/debug.h"
#include "libraries/time/rtc_framework/rtc_framework.h"
#include "libraries/time/ds1302/ds1302.h"

void main(void)
{
    ds1302_t chip;
    rtc_t rtc;
    rtc_datetime_t dt;
    ds1302_status_t st;
    uint8_t ram_value;
    uint32_t unix_time;

    chip.ce_port = &PORTB;
    chip.ce_tris = &TRISB;
    chip.ce_pin = 0u;
    chip.clk_port = &PORTB;
    chip.clk_tris = &TRISB;
    chip.clk_pin = 1u;
    chip.io_port = &PORTB;
    chip.io_tris = &TRISB;
    chip.io_pin = 2u;

    if (ds1302_init(&chip) == 0u)
    {
        DBG_PRINTLN("DS1302 init failed");
        while (1)
        {
        }
    }

    (void)rtc_init(&rtc, ds1302_get_driver(), &chip);

    dt.year = 2026u;
    dt.month = 1u;
    dt.day = 1u;
    dt.hour = 10u;
    dt.minute = 15u;
    dt.second = 0u;
    dt.weekday = 4u;
    (void)rtc_set_datetime(&rtc, &dt);

    /* Optional RAM demo. */
    (void)ds1302_write_ram(&chip, 0u, 0xA5u);
    if (ds1302_read_ram(&chip, 0u, &ram_value) == DS1302_STATUS_OK)
    {
        DBG_PRINT("RAM[0]=0x");
        DBG_PRINT_HEX(ram_value);
        DBG_PRINTLN("");
    }

    while (1)
    {
        st = ds1302_get_datetime(&chip, &dt);
        if (st == DS1302_STATUS_OK)
        {
            DBG_PRINT("DS1302: ");
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
        else if (st == DS1302_STATUS_CLOCK_HALTED)
        {
            DBG_PRINTLN("DS1302: CH set (clock halted)");
        }
        else
        {
            DBG_PRINT("DS1302 error: ");
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
