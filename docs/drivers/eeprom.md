[🇺🇦 Ukrainian version](./eeprom.ua.md)

# EEPROM Driver

## Description
Internal EEPROM driver for byte and block non-volatile storage operations.

## API
- `eeprom_init()`
- `eeprom_read_byte()`
- `eeprom_write_byte()`
- `eeprom_update_byte()`
- `eeprom_read_block()`
- `eeprom_write_block()`

## Example
```c
eeprom_write_byte(0x10u, 0x55u);
value = eeprom_read_byte(0x10u);
```

## Notes
- Supports single-byte and block transfers.
- `eeprom_update_byte()` helps reduce unnecessary write wear.

## Dependencies
- `core/compiler.h`
- `core/types.h`
