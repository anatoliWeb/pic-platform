[Ukrainian version](./seven_segment_mux.ua.md)

# Seven Segment Multiplexed Library

## Description
Reusable non-blocking multiplexed seven segment library for multi-digit LED displays.

## Public API
- `seven_segment_mux_init()`
- `seven_segment_mux_set_digit()`
- `seven_segment_mux_set_number()`
- `seven_segment_mux_set_hex()`
- `seven_segment_mux_clear()`
- `seven_segment_mux_set_dot()`
- `seven_segment_mux_update()`

## Supported Configurations
- 2-digit displays
- 4-digit displays
- configurable digit count up to 8 (`SEVEN_SEGMENT_MUX_MAX_DIGITS`)
- common cathode
- common anode

## Configuration Model
`seven_segment_mux_config_t` includes:
- segment bus pins (`segment_config`)
- digit select pins (`digit_pins`)
- digit count (`digit_count`)
- digit enable logic level (`digit_active_level`)
- optional leading-zero suppression (`suppress_leading_zero`)

## Refresh Model (Non-Blocking)
- `seven_segment_mux_update()` refreshes exactly one digit per call.
- No internal blocking delays.
- Call from:
  - main loop
  - timer ISR hook
  - scheduler task

## Multiplexing Overview
1. disable all digit lines
2. set segment pattern for current digit
3. enable current digit line
4. move to next digit index

This sequence helps reduce ghosting artifacts.

## Refresh Timing Recommendations
- For 4 digits, target effective full-frame refresh >= 100 Hz.
- That means calling `update()` at least 400 times per second.
- Higher refresh reduces visible flicker.

## Wiring Notes
- Segment lines A..G(+DP) are shared between all digits.
- Each digit has a dedicated select line.
- Use resistor/current-limiting according to LED and MCU limits.
- Common cathode/anode behavior is handled by configuration.

## Performance Considerations
- Keep `update()` lightweight and frequent.
- Avoid long blocking code around refresh calls.
- If using ISR refresh, keep ISR short.

## Dependencies
- `core/*`
- `drivers/gpio/*`
- `libraries/display/seven_segment/*` (types reuse)