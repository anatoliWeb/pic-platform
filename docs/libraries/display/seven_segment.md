[Ukrainian version](./seven_segment.ua.md)

# Seven Segment Library

## Description
Reusable single-digit seven segment display library.
Supports common cathode and common anode displays with configurable pin mapping.

## Public API
- `seven_segment_init()`
- `seven_segment_set_digit()`
- `seven_segment_set_hex()`
- `seven_segment_set_raw()`
- `seven_segment_clear()`
- `seven_segment_set_dot()`

## Supported Display Types
- Common cathode
- Common anode

## Display Content Support
- digits `0..9`
- hex `A..F`
- minus sign (raw pattern `0x40`)
- blank state (`clear`)

## Segment Naming
Bit order in raw pattern:
- bit0: A
- bit1: B
- bit2: C
- bit3: D
- bit4: E
- bit5: F
- bit6: G

## Configuration Model
Use `seven_segment_config_t`:
- segment pins `seg_a..seg_g`
- optional `dot` pin + `has_dot`
- display type (`SEVEN_SEGMENT_COMMON_CATHODE` or `SEVEN_SEGMENT_COMMON_ANODE`)

## Wiring Example
Typical single digit wiring:
- MCU GPIO -> segment resistors -> A..G (and optional DP)
- Common pin:
  - GND for common cathode
  - VCC for common anode

## GPIO Mapping Example
```c
static const seven_segment_config_t cfg =
{
    { &PORTB, &TRISB, 0u },
    { &PORTB, &TRISB, 1u },
    { &PORTB, &TRISB, 2u },
    { &PORTB, &TRISB, 3u },
    { &PORTB, &TRISB, 4u },
    { &PORTB, &TRISB, 5u },
    { &PORTB, &TRISB, 6u },
    { &PORTB, &TRISB, 7u },
    1u,
    SEVEN_SEGMENT_COMMON_CATHODE
};
```

## Multiplexing Preparation
Current implementation is single-digit.
The API and config model are suitable for future multi-digit multiplexing extension.

## Dependencies
- `core/*`
- `drivers/gpio/*`