#ifndef LIBRARIES_SENSORS_ULTRASONIC_H
#define LIBRARIES_SENSORS_ULTRASONIC_H

#include "core/compiler.h"
#include "core/types.h"

typedef enum
{
    ULTRASONIC_STATUS_OK = 0,
    ULTRASONIC_STATUS_ERROR,
    ULTRASONIC_STATUS_TIMEOUT_WAIT_HIGH,
    ULTRASONIC_STATUS_TIMEOUT_WAIT_LOW,
    ULTRASONIC_STATUS_INVALID_PULSE,
    ULTRASONIC_STATUS_OVERFLOW
} ultrasonic_status_t;

typedef struct
{
    volatile uint8_t* trig_port;
    volatile uint8_t* trig_tris;
    uint8_t trig_pin;

    volatile uint8_t* echo_port;
    volatile uint8_t* echo_tris;
    uint8_t echo_pin;

    uint32_t timeout_us;
    uint32_t min_pulse_us;
    uint32_t max_pulse_us;
} ultrasonic_config_t;

typedef struct
{
    uint32_t echo_time_us;
    uint32_t distance_mm;
    uint8_t valid;
} ultrasonic_data_t;

typedef struct
{
    ultrasonic_config_t config;
    ultrasonic_data_t data;
    uint8_t initialized;
} ultrasonic_t;

uint8_t ultrasonic_init(ultrasonic_t* sensor, const ultrasonic_config_t* config);

ultrasonic_status_t ultrasonic_trigger(ultrasonic_t* sensor);
ultrasonic_status_t ultrasonic_read(ultrasonic_t* sensor);

uint32_t ultrasonic_get_distance_mm(const ultrasonic_t* sensor);
uint32_t ultrasonic_get_distance_cm(const ultrasonic_t* sensor);
uint8_t ultrasonic_is_valid(const ultrasonic_t* sensor);

#endif /* LIBRARIES_SENSORS_ULTRASONIC_H */
