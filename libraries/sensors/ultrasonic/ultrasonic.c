#include "libraries/sensors/ultrasonic/ultrasonic.h"

#include "core/delay.h"
#include "drivers/gpio/gpio.h"

static uint32_t ultrasonic_default_timeout_us(void)
{
    return 30000u;
}

static uint32_t ultrasonic_default_min_pulse_us(void)
{
    return 100u;
}

static uint32_t ultrasonic_default_max_pulse_us(void)
{
    return 25000u;
}

static uint8_t ultrasonic_wait_level(ultrasonic_t* sensor, uint8_t level, uint32_t timeout_us, uint32_t* elapsed_us)
{
    uint32_t t;

    for (t = 0u; t < timeout_us; t++)
    {
        if (gpio_read(sensor->config.echo_port, sensor->config.echo_pin) == level)
        {
            *elapsed_us = t;
            return 1u;
        }
        DRV_DELAY_US(1u);
    }

    *elapsed_us = timeout_us;
    return 0u;
}

static uint8_t ultrasonic_wait_level_change(ultrasonic_t* sensor,
                                            uint8_t level,
                                            uint32_t timeout_us,
                                            uint32_t* pulse_us)
{
    uint32_t t;

    for (t = 0u; t < timeout_us; t++)
    {
        if (gpio_read(sensor->config.echo_port, sensor->config.echo_pin) != level)
        {
            *pulse_us = t;
            return 1u;
        }
        DRV_DELAY_US(1u);
    }

    *pulse_us = timeout_us;
    return 0u;
}

uint8_t ultrasonic_init(ultrasonic_t* sensor, const ultrasonic_config_t* config)
{
    if ((sensor == (ultrasonic_t*)0) || (config == (const ultrasonic_config_t*)0))
    {
        return 0u;
    }

    if ((config->trig_port == (volatile uint8_t*)0) || (config->trig_tris == (volatile uint8_t*)0))
    {
        return 0u;
    }
    if ((config->echo_port == (volatile uint8_t*)0) || (config->echo_tris == (volatile uint8_t*)0))
    {
        return 0u;
    }

    sensor->config = *config;
    sensor->data.echo_time_us = 0u;
    sensor->data.distance_mm = 0u;
    sensor->data.valid = 0u;
    sensor->initialized = 1u;

    if (sensor->config.timeout_us == 0u)
    {
        sensor->config.timeout_us = ultrasonic_default_timeout_us();
    }
    if (sensor->config.min_pulse_us == 0u)
    {
        sensor->config.min_pulse_us = ultrasonic_default_min_pulse_us();
    }
    if (sensor->config.max_pulse_us == 0u)
    {
        sensor->config.max_pulse_us = ultrasonic_default_max_pulse_us();
    }

    /* Trigger output low in idle. */
    gpio_set_output(sensor->config.trig_tris, sensor->config.trig_pin);
    gpio_write_low(sensor->config.trig_port, sensor->config.trig_pin);

    /* Echo as input. */
    gpio_set_input(sensor->config.echo_tris, sensor->config.echo_pin);

    return 1u;
}

ultrasonic_status_t ultrasonic_trigger(ultrasonic_t* sensor)
{
    if ((sensor == (ultrasonic_t*)0) || (sensor->initialized == 0u))
    {
        return ULTRASONIC_STATUS_ERROR;
    }

    /* HC-SR04 trigger: 10us HIGH pulse. */
    gpio_write_low(sensor->config.trig_port, sensor->config.trig_pin);
    DRV_DELAY_US(2u);
    gpio_write_high(sensor->config.trig_port, sensor->config.trig_pin);
    DRV_DELAY_US(10u);
    gpio_write_low(sensor->config.trig_port, sensor->config.trig_pin);

    return ULTRASONIC_STATUS_OK;
}

ultrasonic_status_t ultrasonic_read(ultrasonic_t* sensor)
{
    uint32_t timeout_us;
    uint32_t pulse_us;
    uint32_t dummy_us;
    ultrasonic_status_t st;

    if ((sensor == (ultrasonic_t*)0) || (sensor->initialized == 0u))
    {
        return ULTRASONIC_STATUS_ERROR;
    }

    sensor->data.valid = 0u;
    sensor->data.echo_time_us = 0u;
    sensor->data.distance_mm = 0u;

    st = ultrasonic_trigger(sensor);
    if (st != ULTRASONIC_STATUS_OK)
    {
        return st;
    }

    timeout_us = sensor->config.timeout_us;

    /* Wait until ECHO goes HIGH. */
    if (ultrasonic_wait_level(sensor, 1u, timeout_us, &dummy_us) == 0u)
    {
        return ULTRASONIC_STATUS_TIMEOUT_WAIT_HIGH;
    }

    /* Measure HIGH pulse width until ECHO returns LOW. */
    if (ultrasonic_wait_level_change(sensor, 1u, timeout_us, &pulse_us) == 0u)
    {
        return ULTRASONIC_STATUS_TIMEOUT_WAIT_LOW;
    }

    if (pulse_us < sensor->config.min_pulse_us)
    {
        return ULTRASONIC_STATUS_INVALID_PULSE;
    }

    if (pulse_us > sensor->config.max_pulse_us)
    {
        return ULTRASONIC_STATUS_OVERFLOW;
    }

    sensor->data.echo_time_us = pulse_us;

    /*
     * Distance one-way:
     * 343 m/s => 0.343 mm/us
     * distance_mm = pulse_us * 0.343 / 2 ~= pulse_us * 343 / 2000
     */
    sensor->data.distance_mm = (uint32_t)(((pulse_us * 343u) + 1000u) / 2000u);
    sensor->data.valid = 1u;

    return ULTRASONIC_STATUS_OK;
}

uint32_t ultrasonic_get_distance_mm(const ultrasonic_t* sensor)
{
    if ((sensor == (const ultrasonic_t*)0) || (sensor->data.valid == 0u))
    {
        return 0u;
    }
    return sensor->data.distance_mm;
}

uint32_t ultrasonic_get_distance_cm(const ultrasonic_t* sensor)
{
    if ((sensor == (const ultrasonic_t*)0) || (sensor->data.valid == 0u))
    {
        return 0u;
    }
    return (uint32_t)(sensor->data.distance_mm / 10u);
}

uint8_t ultrasonic_is_valid(const ultrasonic_t* sensor)
{
    if (sensor == (const ultrasonic_t*)0)
    {
        return 0u;
    }
    return sensor->data.valid;
}
