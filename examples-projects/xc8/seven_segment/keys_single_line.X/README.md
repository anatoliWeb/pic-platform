# keys_single_line.X

[Ukrainian version](./README.ua.md)

## Purpose

Shared-line button example that combines segment-bus key decoding with buffered number formatting
and one-digit blinking.

## What It Demonstrates

- `segment_keys` exact-mask decoding on the shared segment bus
- `button` click, double-click, and hold-repeat events from external raw states
- formatted four-digit value rendering
- leading-zero toggle
- left/right alignment toggle
- blinking active edit digit
- brightness changes without stopping refresh

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
- DOWN: segment mask `0x02`
- OK: segment mask `0x04`

## Dynamic Behavior

- UP and DOWN change the formatted value using the currently blinking edit digit weight
- the active digit moves automatically across the four display positions
- OK click toggles leading-zero mode
- OK double-click toggles left/right alignment
- OK hold-repeat cycles brightness presets

## Notes

- Refresh stays manual through `seven_segment_process()`.
- `tick_get()` provides the non-blocking time base for button logic and blink timing.
- This is an API demonstration only, not a menu system.

## Status

Ready for Proteus validation.
