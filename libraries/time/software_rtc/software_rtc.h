/*
 * File: libraries/time/software_rtc/software_rtc.h
 */

#ifndef LIBRARIES_TIME_SOFTWARE_RTC_H
#define LIBRARIES_TIME_SOFTWARE_RTC_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/time/rtc_framework/rtc_framework.h"

typedef struct
{
    rtc_datetime_t datetime;
    uint16_t ms_accum;
    uint8_t running;
} software_rtc_t;

uint8_t software_rtc_init(software_rtc_t* sw, const rtc_datetime_t* initial_dt);

void software_rtc_tick_ms(software_rtc_t* sw, uint16_t elapsed_ms);
void software_rtc_tick_seconds(software_rtc_t* sw, uint32_t elapsed_seconds);

uint8_t software_rtc_set_datetime(software_rtc_t* sw, const rtc_datetime_t* dt);
uint8_t software_rtc_get_datetime(software_rtc_t* sw, rtc_datetime_t* dt);

void software_rtc_sync(software_rtc_t* sw, const rtc_datetime_t* dt);
uint8_t software_rtc_is_running(const software_rtc_t* sw);

const rtc_driver_t* software_rtc_get_driver(void);

#endif /* LIBRARIES_TIME_SOFTWARE_RTC_H */
