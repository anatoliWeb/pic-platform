# multiplex_manual.X

[Ukrainian version](./README.ua.md)

## Purpose

Manual-refresh four-digit example focused on fixed-point decimal formatting for PIC18F452.

## What It Demonstrates

- `seven_segment_process()` from the main loop
- display buffer separated from GPIO refresh
- `seven_segment_set_fixed_formatted()`
- fixed-point rendering without `float`
- decimal-point placement from `scaled_value` and `decimal_places`
- values below `1.0`, including `0.05`
- negative fixed-point value example
- fixed 100% brightness with blinking disabled for an easier first validation pass
- RE0 error indication and RE1 heartbeat output

## Source Files

- `main.c`
- `config_bits.c`
- `../../../../core/delay.c`
- `../../../../drivers/gpio/gpio.c`
- `../../../../libraries/display/seven_segment/seven_segment.c`

## Pin Mapping

- RD0..RD6: shared segments A..G
- RD7: shared decimal point
- RC0..RC3: digit enable lines
- RE0: error output
- RE1: heartbeat output

## Display Type

- Common cathode

## Refresh Mode

- Manual
- Refresh function: `seven_segment_process()`
- Timer interrupt: not used

## Fixed-point API

- `scaled_value = 1234`, `decimal_places = 2` -> `12.34`
- `scaled_value = 75`, `decimal_places = 1` -> `7.5`
- `scaled_value = 5`, `decimal_places = 2` -> `0.05`

The example keeps the decimal path integer-only and does not use floating-point formatting.

## Dynamic Behavior

The example rotates through prepared fixed-point scenes automatically:

1. `0.00`
2. `0.25`
3. `0.50`
4. `0.75`
5. `1.00`
6. `1.25`
7. `12.34`
8. `7.5`
9. `0.05`
10. zero-padded `00.05`
11. `-12.3`

RE1 toggles whenever the displayed scene changes.
RE0 stays low during normal operation and goes high if formatting or initialization fails.

## Brightness

- Fixed at `100%`
- Blinking disabled

## Troubleshooting

- If flicker is visible, the main loop is too slow for stable manual multiplex refresh.
- If the decimal point appears on the wrong digit, verify the physical DP wiring to RD7 and the digit order on RC0..RC3.
- If RE0 goes high, inspect the fixed-point configuration or display setup.

## Status

Ready for Proteus validation.
