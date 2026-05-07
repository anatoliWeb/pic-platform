#ifndef LIBRARIES_SENSORS_DHT_H
#define LIBRARIES_SENSORS_DHT_H

#include "core/compiler.h"
#include "core/types.h"

typedef enum
{
    DHT_SENSOR_DHT11 = 0,
    DHT_SENSOR_DHT22
} dht_sensor_type_t;

typedef enum
{
    DHT_STATUS_OK = 0,
    DHT_STATUS_ERROR,
    DHT_STATUS_TIMEOUT,
    DHT_STATUS_CHECKSUM,
    DHT_STATUS_INVALID_FRAME,
    DHT_STATUS_TOO_SOON
} dht_status_t;

typedef struct
{
    volatile uint8_t* port;
    volatile uint8_t* tris;
    uint8_t pin;

    dht_sensor_type_t sensor_type;

    /* Optional custom timings. 0 = use defaults by sensor type. */
    uint16_t start_low_ms;
    uint16_t min_interval_ms;
    uint16_t bit_timeout_us;
} dht_config_t;

typedef struct
{
    int16_t temperature_x10;
    uint16_t humidity_x10;
    uint8_t valid;
} dht_data_t;

typedef struct
{
    dht_config_t config;
    dht_data_t data;
    uint32_t last_read_ms;
    uint8_t initialized;
} dht_t;

uint8_t dht_init(dht_t* dht, const dht_config_t* config);
dht_status_t dht_read(dht_t* dht);

int16_t dht_get_temperature(const dht_t* dht);
uint16_t dht_get_humidity(const dht_t* dht);

uint8_t dht_is_valid(const dht_t* dht);

#endif /* LIBRARIES_SENSORS_DHT_H */
