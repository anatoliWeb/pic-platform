[🇺🇦 Ukrainian version](./clock.ua.md)

# Clock Helper

## Description
Central clock-frequency helper used as source of truth for timing-sensitive drivers.

## API
- `CLOCK_FREQUENCY`
- `clock_get_frequency()`

## Example
```c
freq = clock_get_frequency();
```

## Notes
- Frequency is defined by `DRV_XTAL_FREQ` in device/core config.
- Affects delay, UART, I2C, and other timing calculations.

## Dependencies
- `core/compiler.h`
- `core/device.h`
