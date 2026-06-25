/*
 * File: libraries/time/timezone/timezone.h
 */

#ifndef LIBRARIES_TIME_TIMEZONE_H
#define LIBRARIES_TIME_TIMEZONE_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/time/rtc_framework/rtc_framework.h"

typedef struct
{
    int16_t offset_minutes;
    uint8_t dst_enabled;
    uint8_t initialized;
} timezone_t;

uint8_t timezone_init(timezone_t* tz);
void timezone_set_offset_minutes(timezone_t* tz, int16_t offset_minutes);
void timezone_set_dst(timezone_t* tz, uint8_t enabled);

int16_t timezone_get_offset_minutes(const timezone_t* tz);
uint8_t timezone_is_dst_enabled(const timezone_t* tz);

uint8_t timezone_utc_to_local(const timezone_t* tz, const rtc_datetime_t* utc_dt, rtc_datetime_t* local_dt);
uint8_t timezone_local_to_utc(const timezone_t* tz, const rtc_datetime_t* local_dt, rtc_datetime_t* utc_dt);

#endif /* LIBRARIES_TIME_TIMEZONE_H */
