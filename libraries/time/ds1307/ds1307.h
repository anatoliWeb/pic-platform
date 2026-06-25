/*
 * File: libraries/time/ds1307/ds1307.h
 */

#ifndef LIBRARIES_TIME_DS1307_H
#define LIBRARIES_TIME_DS1307_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/time/rtc_framework/rtc_framework.h"

typedef enum
{
    DS1307_STATUS_OK = 0,
    DS1307_STATUS_ERROR,
    DS1307_STATUS_I2C_ERROR,
    DS1307_STATUS_INVALID_DATETIME,
    DS1307_STATUS_OSCILLATOR_HALTED,
    DS1307_STATUS_INVALID_RAM_ADDRESS
} ds1307_status_t;

typedef struct
{
    uint8_t i2c_address;
    uint8_t initialized;
} ds1307_t;

uint8_t ds1307_init(ds1307_t* rtc, uint8_t i2c_address);

ds1307_status_t ds1307_set_datetime(ds1307_t* rtc, const rtc_datetime_t* dt);
ds1307_status_t ds1307_get_datetime(ds1307_t* rtc, rtc_datetime_t* dt);

ds1307_status_t ds1307_set_ram(ds1307_t* rtc, uint8_t offset, uint8_t value);
ds1307_status_t ds1307_get_ram(ds1307_t* rtc, uint8_t offset, uint8_t* value);

const rtc_driver_t* ds1307_get_driver(void);

#endif /* LIBRARIES_TIME_DS1307_H */
