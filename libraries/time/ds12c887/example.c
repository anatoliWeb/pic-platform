#include "core/compiler.h"
#include "core/debug.h"
#include "libraries/time/rtc_framework/rtc_framework.h"
#include "libraries/time/ds12c887/ds12c887.h"

void main(void)
{
    ds12c887_t chip;
    rtc_t rtc;
    rtc_datetime_t dt;
    ds12c887_status_t st;
    uint8_t cmos_value;
    uint32_t unix_time;

    /* Example wiring: address/data buses on full ports, control lines on bits. */
    chip.addr_port = &PORTD;
    chip.addr_tris = &TRISD;
    chip.data_port = &PORTC;
    chip.data_tris = &TRISC;
    chip.ctrl_port = &PORTB;
    chip.ctrl_tris = &TRISB;
    chip.pin_cs = 0u;
    chip.pin_as = 1u;
    chip.pin_ds = 2u;
    chip.pin_rw = 3u;
    chip.century_reg = 0x32u;
    chip.uip_timeout_us = 2000u;

    if (ds12c887_init(&chip) == 0u)
    {
        DBG_PRINTLN("DS12C887 init failed");
        while (1)
        {
        }
    }

    (void)rtc_init(&rtc, ds12c887_get_driver(), &chip);

    dt.year = 2026u;
    dt.month = 1u;
    dt.day = 1u;
    dt.hour = 12u;
    dt.minute = 0u;
    dt.second = 0u;
    dt.weekday = 4u;
    (void)rtc_set_datetime(&rtc, &dt);

    /* CMOS RAM demo. */
    (void)ds12c887_write_cmos(&chip, 0x20u, 0x5Au);
    if (ds12c887_read_cmos(&chip, 0x20u, &cmos_value) != 0u)
    {
        DBG_PRINT("CMOS[0x20]=0x");
        DBG_PRINT_HEX(cmos_value);
        DBG_PRINTLN("");
    }

    while (1)
    {
        st = ds12c887_get_datetime(&chip, &dt);
        if (st == DS12C887_STATUS_OK)
        {
            DBG_PRINT("DS12C887: ");
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
        else
        {
            DBG_PRINT("DS12C887 error: ");
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
