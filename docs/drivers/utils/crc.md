[🇺🇦 Ukrainian version](./crc.ua.md)

# CRC Driver

## Description

Generic data integrity helper for communication/storage drivers.
Provides CRC8, CRC16, and checksum routines.

## API

- `crc8_dallas()`
- `crc16_modbus()`
- `checksum8()`

## Example

```c
uint8_t frame[] = {0xAAu, 0x03u, 0x10u, 0x20u};
uint16_t crc = crc16_modbus(frame, 4u);

if (crc16_modbus(frame, 4u) == crc)
{
    // frame valid
}
```

## Notes

- Loop-based implementation (no lookup table).
- Lightweight and portable for embedded targets.
- Used by protocol layers such as RS485 and 1-Wire-related checks.

## Dependencies

- `core/compiler.h`
- `core/types.h`

