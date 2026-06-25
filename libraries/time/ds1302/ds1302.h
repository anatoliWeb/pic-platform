/*
 * File: libraries/time/ds1302/ds1302.h
 */

#ifndef LIBRARIES_TIME_DS1302_H
#define LIBRARIES_TIME_DS1302_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/time/rtc_framework/rtc_framework.h"

typedef enum
{
    DS1302_STATUS_OK = 0,
    DS1302_STATUS_ERROR,
    DS1302_STATUS_INVALID_DATETIME,
    DS1302_STATUS_CLOCK_HALTED,
    DS1302_STATUS_INVALID_RAM_ADDRESS
} ds1302_status_t;

typedef struct
{
    volatile uint8_t* ce_port;
    volatile uint8_t* ce_tris;
    uint8_t ce_pin;

    volatile uint8_t* clk_port;
    volatile uint8_t* clk_tris;
    uint8_t clk_pin;

    volatile uint8_t* io_port;
    volatile uint8_t* io_tris;
    uint8_t io_pin;

    uint8_t initialized;
} ds1302_t;

uint8_t ds1302_init(ds1302_t* rtc);

ds1302_status_t ds1302_set_datetime(ds1302_t* rtc, const rtc_datetime_t* dt);
ds1302_status_t ds1302_get_datetime(ds1302_t* rtc, rtc_datetime_t* dt);

ds1302_status_t ds1302_write_ram(ds1302_t* rtc, uint8_t offset, uint8_t value);
ds1302_status_t ds1302_read_ram(ds1302_t* rtc, uint8_t offset, uint8_t* value);

void ds1302_enable_write_protect(ds1302_t* rtc);
void ds1302_disable_write_protect(ds1302_t* rtc);

const rtc_driver_t* ds1302_get_driver(void);

#endif /* LIBRARIES_TIME_DS1302_H */
