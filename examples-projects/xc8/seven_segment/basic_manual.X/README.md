# basic_manual.X

## Purpose

Single-digit seven-segment example with manual refresh from the main loop.

## Refresh Mode

- Manual
- Refresh function: `seven_segment_process()`
- Timer driver: not used for display refresh

## Source Files

- `main.c`
- `config_bits.c`
- `../../../../core/delay.c`
- `../../../../drivers/gpio/gpio.c`
- `../../../../libraries/display/seven_segment/seven_segment.c`

## Pin Mapping

- RD0..RD6: segments A..G
- RD7: decimal point
- RC0: digit enable

## Display Type

- Common cathode

## Dynamic Behavior

- Auto counts `0 -> 9 -> 0`
- Decimal point toggles periodically
- Global brightness ramps from low to high and back

## Brightness

- Uses `seven_segment_set_brightness()`
- Example range: `10..100`

## Proteus Wiring

- PIC18F452
- 10 MHz crystal
- Single-digit common-cathode display
- Current-limit resistors on all segment lines

## Troubleshooting

- If the digit flickers, call `seven_segment_process()` more often.
- Long blocking code in the main loop reduces refresh quality.

## Status

Ready for Proteus validation.
