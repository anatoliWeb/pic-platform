# DHT Sensor Library (DHT11 + DHT22)

## Description

`dht` is a unified sensor library for DHT11 and DHT22.
The sensor type is selected by configuration, so a single API works for both devices.

## Public API

- `dht_init()`
- `dht_read()`
- `dht_get_temperature()` (x10 scale)
- `dht_get_humidity()` (x10 scale)
- `dht_is_valid()`

## Supported Sensor Types

- `DHT_SENSOR_DHT11`
- `DHT_SENSOR_DHT22`

## Data Model

The library stores parsed values in:

- `temperature_x10` (for example `253` = 25.3C)
- `humidity_x10` (for example `615` = 61.5%)
- `valid` flag

## Timing and Protocol Notes

The implementation performs standard DHT single-wire exchange:

1. Host start pulse (`~18ms` DHT11, `~1ms` DHT22 by default)
2. Sensor response pulse pair
3. 40-bit frame read with pulse-width decoding
4. Checksum verification

Timing-sensitive read section is compact and protected by interrupt disable/enable.

## Error Handling

`dht_read()` returns `dht_status_t`:

- `DHT_STATUS_OK`
- `DHT_STATUS_TIMEOUT`
- `DHT_STATUS_CHECKSUM`
- `DHT_STATUS_INVALID_FRAME`
- `DHT_STATUS_TOO_SOON`
- `DHT_STATUS_ERROR`

## DHT11 vs DHT22

- DHT11: lower precision, integer-focused values, shorter minimum period.
- DHT22: better precision, signed temperature, recommended 2s read interval.

## Wiring Notes

- Data line requires pull-up resistor (typical 4.7k to VCC).
- Keep line short and avoid noisy routing.
- Ensure GPIO pin can be switched input/output.

## Refresh Rate Limits

- DHT11 default minimum interval: 1 second
- DHT22 default minimum interval: 2 seconds

Intervals can be overridden in config when needed.

## Dependencies

- `core/*`
- `drivers/gpio`
- `drivers/timers/tick`
