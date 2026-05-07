[🇺🇦 Ukrainian version](./i2c.ua.md)

# I2C Driver

## Description

I2C driver based on PIC MSSP module for master mode communication.
Used for device discovery and register-level communication with sensors and peripherals.

## API

- `i2c_init()`
- `i2c_start()`
- `i2c_stop()`
- `i2c_write_byte()`
- `i2c_read_byte()`
- `i2c_scan()`

## Example

```c
#include "drivers/communication/i2c/i2c.h"

void main(void)
{
    uint8_t found[16];
    uint8_t reg_val = 0u;

    i2c_init(100000u);
    (void)i2c_scan(found, 16u);
    (void)i2c_read_register(0x48u, 0x00u, &reg_val);
}
```

## Notes

- ACK/NACK handling is part of byte-level read/write flow.
- Current implementation targets master mode.
- Bus speed and timing depend on clock configuration.

## Dependencies

- `core/compiler.h`
- `core/types.h`
- `core/device.h`
- `drivers/clock` (frequency source for timing calculations)

