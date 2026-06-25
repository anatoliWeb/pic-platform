/*
 * File: libraries/time/ds3231/ds3231.h
 */

#ifndef LIBRARIES_TIME_DS3231_H
#define LIBRARIES_TIME_DS3231_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/time/rtc_framework/rtc_framework.h"

typedef enum
{
    DS3231_STATUS_OK = 0,
    DS3231_STATUS_ERROR,
    DS3231_STATUS_I2C_ERROR,
    DS3231_STATUS_INVALID_DATETIME,
    DS3231_STATUS_OSCILLATOR_STOPPED
} ds3231_status_t;

typedef struct
{
    uint8_t i2c_address;
    uint8_t initialized;
} ds3231_t;

uint8_t ds3231_init(ds3231_t* rtc, uint8_t i2c_address);

ds3231_status_t ds3231_set_datetime(ds3231_t* rtc, const rtc_datetime_t* dt);
ds3231_status_t ds3231_get_datetime(ds3231_t* rtc, rtc_datetime_t* dt);

int16_t ds3231_get_temperature(ds3231_t* rtc);

const rtc_driver_t* ds3231_get_driver(void);

#endif /* LIBRARIES_TIME_DS3231_H */
