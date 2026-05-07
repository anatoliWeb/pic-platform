[🇺🇦 Ukrainian version](./i2c_eeprom.ua.md)

# I2C EEPROM Driver

## Description
Generic external EEPROM driver for 24Cxx family devices over I2C.
This is a memory driver layer and depends only on core + I2C communication driver.

## Supported EEPROM Family
- 24C01
- 24C02
- 24C04
- 24C08
- 24C16
- 24C32
- 24C64
- 24C256

## Public API
- `i2c_eeprom_init()`
- `i2c_eeprom_is_ready()`
- `i2c_eeprom_write_byte()`
- `i2c_eeprom_read_byte()`
- `i2c_eeprom_write_buffer()`
- `i2c_eeprom_read_buffer()`

## Configuration Model
Use `i2c_eeprom_config_t`:
- `device_address`: 7-bit base EEPROM address (typically `0x50`)
- `page_size`: page write size in bytes (8/16/32/64)
- `address_width`:
  - `I2C_EEPROM_ADDR_WIDTH_8`
  - `I2C_EEPROM_ADDR_WIDTH_16`
- `ready_retries`: max write-cycle polling retries
- `write_cycle_ms`: delay between ready checks

## Addressing Notes
- 8-bit memory address mode (small 24Cxx) uses control-address block bits.
- 16-bit memory address mode (24C32+) sends high+low memory address bytes.

## Page Write Limitation
`i2c_eeprom_write_buffer()` automatically splits writes by page boundary.
Cross-page writes are sent in multiple transactions.

## Wiring (Typical)
- VCC -> 3.3V/5V
- GND -> GND
- SDA -> MCU SDA
- SCL -> MCU SCL
- WP -> GND (enable writes) or VCC (write protect)
- A0/A1/A2 -> address select pins

## Example
```c
#include "drivers/communication/i2c/i2c.h"
#include "drivers/memory/i2c_eeprom/i2c_eeprom.h"

i2c_eeprom_config_t cfg = {0x50u, 16u, I2C_EEPROM_ADDR_WIDTH_8, 40u, 5u};
uint8_t value;

i2c_init(100000u);
i2c_eeprom_init(&cfg);

i2c_eeprom_write_byte(0x0000u, 0x5Au);
i2c_eeprom_read_byte(0x0000u, &value);
```

## Dependencies
- `core/*`
- `drivers/communication/i2c/*`