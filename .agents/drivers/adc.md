# adc

## Purpose

ADC driver used by sensor-reading examples and `position_drive`.

## Location

```text
drivers/analog/adc/adc.h
drivers/analog/adc/adc.c
```

## Public API

| Function/type | Purpose | What to pass | Returns / output | Notes |
| --- | --- | --- | --- | --- |
| `adc_init()` | initialize ADC | nothing | none | project config decides analog setup |
| `adc_read()` | read one channel | channel number | `uint16_t` sample | used by position drive |
| `adc_read_avg()` | average several samples | channel, sample count | `uint16_t` sample | helper for smoother reads |
| `adc_read_multiple()` | batch read multiple channels | channel list, result array, count | none | convenience helper |
| `adc_to_millivolts()` | convert sample to mV | sample, Vref mV | `uint16_t` mv | helper |
| `adc_read_voltage()` | read and convert voltage | channel | `uint16_t` mv | helper |
| `adc_read_thermistor()` | thermistor helper | channel | `int16_t` | sensor-specific helper |
| `adc_read_button()` | button threshold helper | channel | `uint8_t` | convenience helper |

## Resource ownership

- ADC channels are project-owned.
- `position_drive` uses ADC through callbacks, not by direct hardware access.
