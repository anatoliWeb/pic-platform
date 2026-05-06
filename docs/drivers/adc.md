[🇺🇦 Ukrainian version](./adc.ua.md)

# ADC Driver

## Description

Analog-to-Digital Converter (ADC) driver for PIC18 platform.
Used for sensor input, ADC ladder buttons, and general analog measurements.

## API

- `adc_init()`
- `adc_read()`
- `adc_read_avg()`
- `adc_read_multiple()`

## Example

```c
#include "drivers/adc/adc.h"

void main(void)
{
    uint16_t pot;
    int16_t therm;

    adc_init();

    pot = adc_read(0u);            // potentiometer
    therm = adc_read_thermistor(1u); // thermistor helper
}
```

## Notes

- 10-bit ADC conversion result.
- Read operations are blocking.
- Conversion timing depends on MCU clock and ADC configuration.

## Dependencies

- `core/compiler.h`
- `core/types.h`
- `core/device.h`

