[🇺🇦 Ukrainian version](./onewire.ua.md)

# OneWire Driver

## Description

Bit-banged 1-Wire base driver over GPIO.
Implements bus reset/presence, bit/byte transfer, and ROM-level commands.

## API

- `onewire_init()`
- `onewire_reset()`
- `onewire_write_bit()`
- `onewire_read_bit()`
- `onewire_write_byte()`
- `onewire_read_byte()`
- `onewire_skip_rom()`
- `onewire_match_rom()`
- `onewire_read_rom()`
- `onewire_search_rom()` - performs Dallas/Maxim Search ROM and returns discovered ROM codes
- `onewire_crc8()`

## Example

```c
uint8_t rom[8];

onewire_init(&PORTB, &TRISB, 0u);
if (onewire_reset() != 0u)
{
    onewire_read_rom(rom);
}
```

## Notes

- Timing-sensitive protocol; uses microsecond delays.
- ROM commands support single-device and addressed-device flows.
- `onewire_search_rom()` supports multi-drop enumeration on a shared 1-Wire bus.

## Dependencies

- `core/compiler.h`
- `core/types.h`
- `core/delay.h`
- `core/interrupts.h`
