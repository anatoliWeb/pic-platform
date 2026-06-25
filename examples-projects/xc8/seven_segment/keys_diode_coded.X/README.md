# keys_diode_coded.X

[Ukrainian version](./README.ua.md)

## Purpose

Shared-line diode-coded key example for blink-mask selection and switching between formatted number mode and `HHMM` mode.

## What It Demonstrates

- diode-coded exact masks with `segment_keys`
- no buffer change for unknown or ambiguous masks
- switching between numeric display and `HHMM` display
- single-digit and multi-digit blink masks
- optional leading zero for hours in time mode
- reuse of the same display buffer for number and time scenes

## Source Files

- `main.c`
- `config_bits.c`
- `../../../../core/delay.c`
- `../../../../drivers/gpio/gpio.c`
- `../../../../drivers/timers/tick/tick.c`
- `../../../../drivers/timers/timer1/timer1.c`
- `../../../../libraries/display/seven_segment/seven_segment.c`
- `../../../../libraries/input/button/button.c`
- `../../../../libraries/input/segment_keys/segment_keys.c`

## Pin Mapping

- RD0..RD6: shared segments A..G
- RD7: shared decimal point
- RC0..RC3: digit enable lines

## Logical Keys

- UP: segment mask `0x01`
- DOWN: segment mask `0x03`
- OK: segment mask `0x07`

## Dynamic Behavior

- UP cycles through predefined blink masks
- DOWN switches between numeric mode and `HHMM` mode
- OK updates the current scene without clearing the display buffer model
- invalid masks are ignored and do not generate logical button events

## Notes

- In time mode the colon effect uses decimal points on the middle digits.
- In number mode the example keeps right-aligned formatting.
- This example intentionally leaves ambiguous masks as no-op conditions.

## Status

Ready for Proteus validation.
