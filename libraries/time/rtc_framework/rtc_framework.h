#ifndef LIBRARIES_TIME_RTC_FRAMEWORK_H
#define LIBRARIES_TIME_RTC_FRAMEWORK_H

#include "core/compiler.h"
#include "core/types.h"

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;

    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    uint8_t weekday;
} rtc_datetime_t;

typedef struct
{
    uint8_t (*set_datetime)(void* ctx, const rtc_datetime_t* dt);
    uint8_t (*get_datetime)(void* ctx, rtc_datetime_t* dt);
} rtc_driver_t;

typedef struct
{
    rtc_driver_t driver;
    void* driver_ctx;

    rtc_datetime_t soft_dt;
    uint32_t soft_unix;

    uint8_t use_soft_rtc;
    uint8_t initialized;
} rtc_t;

uint8_t rtc_init(rtc_t* rtc, const rtc_driver_t* driver, void* driver_ctx);

uint8_t rtc_set_datetime(rtc_t* rtc, const rtc_datetime_t* dt);
uint8_t rtc_get_datetime(rtc_t* rtc, rtc_datetime_t* dt);

uint8_t rtc_set_unix(rtc_t* rtc, uint32_t unix_time);
uint8_t rtc_get_unix(rtc_t* rtc, uint32_t* unix_time);

void rtc_tick(rtc_t* rtc);
uint8_t rtc_is_valid(const rtc_datetime_t* dt);

#endif /* LIBRARIES_TIME_RTC_FRAMEWORK_H */
