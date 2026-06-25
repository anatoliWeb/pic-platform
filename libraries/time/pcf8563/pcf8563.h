/*
 * File: libraries/time/pcf8563/pcf8563.h
 */

#ifndef LIBRARIES_TIME_PCF8563_H
#define LIBRARIES_TIME_PCF8563_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/time/rtc_framework/rtc_framework.h"

typedef enum
{
    PCF8563_STATUS_OK = 0,
    PCF8563_STATUS_ERROR,
    PCF8563_STATUS_I2C_ERROR,
    PCF8563_STATUS_INVALID_DATETIME,
    PCF8563_STATUS_VOLTAGE_LOW
} pcf8563_status_t;

typedef struct
{
    uint8_t i2c_address;
    uint8_t initialized;
} pcf8563_t;

uint8_t pcf8563_init(pcf8563_t* rtc, uint8_t i2c_address);

pcf8563_status_t pcf8563_set_datetime(pcf8563_t* rtc, const rtc_datetime_t* dt);
pcf8563_status_t pcf8563_get_datetime(pcf8563_t* rtc, rtc_datetime_t* dt);

/* Optional helper: set minute/hour/day/weekday alarm (0x80 bit means disabled). */
pcf8563_status_t pcf8563_set_alarm(pcf8563_t* rtc,
                                   int8_t minute,
                                   int8_t hour,
                                   int8_t day,
                                   int8_t weekday);
pcf8563_status_t pcf8563_clear_alarm(pcf8563_t* rtc);

const rtc_driver_t* pcf8563_get_driver(void);

#endif /* LIBRARIES_TIME_PCF8563_H */
