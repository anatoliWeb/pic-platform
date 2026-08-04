/*
 * File: libraries/sensors/tachometer/tachometer.h
 */

#ifndef LIBRARIES_SENSORS_TACHOMETER_H
#define LIBRARIES_SENSORS_TACHOMETER_H

#include "core/compiler.h"
#include "core/types.h"

typedef enum
{
    TACHOMETER_STATUS_NOT_INITIALIZED = 0,
    TACHOMETER_STATUS_STOPPED,
    TACHOMETER_STATUS_STARTING,
    TACHOMETER_STATUS_RUNNING,
    TACHOMETER_STATUS_TOO_SLOW,
    TACHOMETER_STATUS_NO_SIGNAL,
    TACHOMETER_STATUS_CONFIG_ERROR
} tachometer_status_t;

typedef struct
{
    uint8_t pulses_per_revolution;
    uint16_t minimum_rpm;
    uint16_t startup_grace_ms;
    uint16_t signal_timeout_ms;
    uint16_t minimum_pulse_interval_us;
} tachometer_config_t;

typedef struct
{
    tachometer_config_t config;
    uint8_t initialized;
    uint8_t expected_running;
    tachometer_status_t status;
    uint32_t expected_running_since_us;
    uint32_t last_pulse_us;
    uint32_t pulse_count;
    uint16_t rpm;
    uint8_t session_pulse_count;
} tachometer_t;

drv_status_t tachometer_init(tachometer_t* tachometer,
                             const tachometer_config_t* config);
void tachometer_set_expected_running(tachometer_t* tachometer,
                                     uint8_t expected_running,
                                     uint32_t now_us);
uint8_t tachometer_on_pulse(tachometer_t* tachometer, uint32_t now_us);
void tachometer_process(tachometer_t* tachometer, uint32_t now_us);
uint16_t tachometer_get_rpm(const tachometer_t* tachometer);
tachometer_status_t tachometer_get_status(const tachometer_t* tachometer);
uint32_t tachometer_get_pulse_count(const tachometer_t* tachometer);
void tachometer_reset(tachometer_t* tachometer);

#endif /* LIBRARIES_SENSORS_TACHOMETER_H */
