#include "libraries/sensors/dht/dht.h"

#include "core/delay.h"
#include "drivers/gpio/gpio.h"
#include "drivers/timers/tick/tick.h"

static uint16_t dht_default_start_low_ms(dht_sensor_type_t type)
{
    if (type == DHT_SENSOR_DHT11)
    {
        return 18u;
    }
    return 1u;
}

static uint16_t dht_default_min_interval_ms(dht_sensor_type_t type)
{
    if (type == DHT_SENSOR_DHT11)
    {
        return 1000u;
    }
    return 2000u;
}

static uint16_t dht_default_bit_timeout_us(void)
{
    return 120u;
}

static uint16_t dht_get_start_low_ms(const dht_t* dht)
{
    if (dht->config.start_low_ms == 0u)
    {
        return dht_default_start_low_ms(dht->config.sensor_type);
    }
    return dht->config.start_low_ms;
}

static uint16_t dht_get_min_interval_ms(const dht_t* dht)
{
    if (dht->config.min_interval_ms == 0u)
    {
        return dht_default_min_interval_ms(dht->config.sensor_type);
    }
    return dht->config.min_interval_ms;
}

static uint16_t dht_get_timeout_us(const dht_t* dht)
{
    if (dht->config.bit_timeout_us == 0u)
    {
        return dht_default_bit_timeout_us();
    }
    return dht->config.bit_timeout_us;
}

static uint8_t dht_wait_level(const dht_t* dht, uint8_t level, uint16_t timeout_us)
{
    uint16_t t;

    for (t = 0u; t < timeout_us; t++)
    {
        if (gpio_read(dht->config.port, dht->config.pin) == level)
        {
            return 1u;
        }
        DRV_DELAY_US(1u);
    }

    return 0u;
}

static uint8_t dht_wait_level_change(const dht_t* dht, uint8_t level, uint16_t timeout_us, uint16_t* pulse_us)
{
    uint16_t t;

    for (t = 0u; t < timeout_us; t++)
    {
        if (gpio_read(dht->config.port, dht->config.pin) != level)
        {
            *pulse_us = t;
            return 1u;
        }
        DRV_DELAY_US(1u);
    }

    *pulse_us = timeout_us;
    return 0u;
}

static dht_status_t dht_read_frame(dht_t* dht, uint8_t frame[5])
{
    uint8_t i;
    uint8_t bit_index;
    uint8_t mask;
    uint16_t pulse_us;
    uint16_t timeout_us;

    for (i = 0u; i < 5u; i++)
    {
        frame[i] = 0u;
    }

    timeout_us = dht_get_timeout_us(dht);

    DRV_INT_DISABLE();

    /* Host start signal: pull low, then release line. */
    gpio_set_output(dht->config.tris, dht->config.pin);
    gpio_write_low(dht->config.port, dht->config.pin);
    DRV_DELAY_MS(dht_get_start_low_ms(dht));
    gpio_set_input(dht->config.tris, dht->config.pin);
    DRV_DELAY_US(30u);

    /* Sensor response: LOW ~80us then HIGH ~80us. */
    if (dht_wait_level(dht, 0u, timeout_us) == 0u)
    {
        DRV_INT_ENABLE();
        return DHT_STATUS_TIMEOUT;
    }
    if (dht_wait_level(dht, 1u, timeout_us) == 0u)
    {
        DRV_INT_ENABLE();
        return DHT_STATUS_TIMEOUT;
    }
    if (dht_wait_level(dht, 0u, timeout_us) == 0u)
    {
        DRV_INT_ENABLE();
        return DHT_STATUS_TIMEOUT;
    }

    /* Read 40 bits: each bit starts with LOW, then HIGH pulse width encodes value. */
    for (i = 0u; i < 5u; i++)
    {
        for (bit_index = 0u; bit_index < 8u; bit_index++)
        {
            if (dht_wait_level(dht, 1u, timeout_us) == 0u)
            {
                DRV_INT_ENABLE();
                return DHT_STATUS_TIMEOUT;
            }

            if (dht_wait_level_change(dht, 1u, timeout_us, &pulse_us) == 0u)
            {
                DRV_INT_ENABLE();
                return DHT_STATUS_TIMEOUT;
            }

            mask = (uint8_t)(1u << (7u - bit_index));
            if (pulse_us > 40u)
            {
                frame[i] |= mask;
            }
        }
    }

    DRV_INT_ENABLE();
    return DHT_STATUS_OK;
}

static dht_status_t dht_decode_frame(dht_t* dht, const uint8_t frame[5])
{
    uint8_t checksum;
    int16_t temp_x10;
    uint16_t hum_x10;
    uint16_t raw_temp;

    checksum = (uint8_t)(frame[0] + frame[1] + frame[2] + frame[3]);
    if (checksum != frame[4])
    {
        dht->data.valid = 0u;
        return DHT_STATUS_CHECKSUM;
    }

    if (dht->config.sensor_type == DHT_SENSOR_DHT11)
    {
        hum_x10 = (uint16_t)((uint16_t)frame[0] * 10u + (uint16_t)(frame[1] % 10u));
        temp_x10 = (int16_t)((int16_t)frame[2] * 10 + (int16_t)(frame[3] % 10u));
    }
    else
    {
        hum_x10 = (uint16_t)(((uint16_t)frame[0] << 8u) | frame[1]);
        raw_temp = (uint16_t)((((uint16_t)frame[2] & 0x7Fu) << 8u) | frame[3]);
        temp_x10 = (int16_t)raw_temp;
        if ((frame[2] & 0x80u) != 0u)
        {
            temp_x10 = (int16_t)(-temp_x10);
        }
    }

    dht->data.humidity_x10 = hum_x10;
    dht->data.temperature_x10 = temp_x10;
    dht->data.valid = 1u;
    return DHT_STATUS_OK;
}

uint8_t dht_init(dht_t* dht, const dht_config_t* config)
{
    if ((dht == (dht_t*)0) || (config == (const dht_config_t*)0))
    {
        return 0u;
    }

    if ((config->port == (volatile uint8_t*)0) || (config->tris == (volatile uint8_t*)0))
    {
        return 0u;
    }

    dht->config = *config;
    dht->data.temperature_x10 = 0;
    dht->data.humidity_x10 = 0u;
    dht->data.valid = 0u;
    dht->last_read_ms = 0u;
    dht->initialized = 1u;

    /* DHT line is idle-high with pull-up. Keep pin released as input. */
    gpio_set_input(dht->config.tris, dht->config.pin);
    return 1u;
}

dht_status_t dht_read(dht_t* dht)
{
    uint8_t frame[5];
    uint32_t now;
    uint16_t min_interval_ms;
    dht_status_t st;

    if ((dht == (dht_t*)0) || (dht->initialized == 0u))
    {
        return DHT_STATUS_ERROR;
    }

    now = tick_get();
    min_interval_ms = dht_get_min_interval_ms(dht);

    if ((dht->last_read_ms != 0u) && ((uint32_t)(now - dht->last_read_ms) < (uint32_t)min_interval_ms))
    {
        return DHT_STATUS_TOO_SOON;
    }

    st = dht_read_frame(dht, frame);
    if (st != DHT_STATUS_OK)
    {
        dht->data.valid = 0u;
        return st;
    }

    st = dht_decode_frame(dht, frame);
    if (st == DHT_STATUS_OK)
    {
        dht->last_read_ms = now;
    }
    return st;
}

int16_t dht_get_temperature(const dht_t* dht)
{
    if ((dht == (const dht_t*)0) || (dht->data.valid == 0u))
    {
        return 0;
    }
    return dht->data.temperature_x10;
}

uint16_t dht_get_humidity(const dht_t* dht)
{
    if ((dht == (const dht_t*)0) || (dht->data.valid == 0u))
    {
        return 0u;
    }
    return dht->data.humidity_x10;
}

uint8_t dht_is_valid(const dht_t* dht)
{
    if (dht == (const dht_t*)0)
    {
        return 0u;
    }
    return dht->data.valid;
}
