/*
 * File: libraries/time/ds12c887/ds12c887.h
 */

#ifndef LIBRARIES_TIME_DS12C887_H
#define LIBRARIES_TIME_DS12C887_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/time/rtc_framework/rtc_framework.h"

typedef enum
{
    DS12C887_STATUS_OK = 0,
    DS12C887_STATUS_ERROR,
    DS12C887_STATUS_INVALID_DATETIME,
    DS12C887_STATUS_UIP_TIMEOUT,
    DS12C887_STATUS_INVALID_DATA
} ds12c887_status_t;

typedef struct
{
    /* 8-bit index/address bus */
    volatile uint8_t* addr_port;
    volatile uint8_t* addr_tris;

    /* 8-bit data bus */
    volatile uint8_t* data_port;
    volatile uint8_t* data_tris;

    /* Control lines on GPIO pins (active-low signals). */
    volatile uint8_t* ctrl_port;
    volatile uint8_t* ctrl_tris;
    uint8_t pin_cs;
    uint8_t pin_as;
    uint8_t pin_ds;
    uint8_t pin_rw;

    uint8_t century_reg;
    uint16_t uip_timeout_us;

    uint8_t initialized;
} ds12c887_t;

uint8_t ds12c887_init(ds12c887_t* rtc);

ds12c887_status_t ds12c887_set_datetime(ds12c887_t* rtc, const rtc_datetime_t* dt);
ds12c887_status_t ds12c887_get_datetime(ds12c887_t* rtc, rtc_datetime_t* dt);

uint8_t ds12c887_read_cmos(ds12c887_t* rtc, uint8_t index, uint8_t* value);
uint8_t ds12c887_write_cmos(ds12c887_t* rtc, uint8_t index, uint8_t value);

void ds12c887_enable_periodic_irq(ds12c887_t* rtc, uint8_t enabled);

const rtc_driver_t* ds12c887_get_driver(void);

#endif /* LIBRARIES_TIME_DS12C887_H */
