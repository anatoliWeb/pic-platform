[Ukrainian version](./shared_segment_buttons.ua.md)

# Shared Segment Buttons Library

## Description
Library for scanning buttons that share IO lines with multiplexed seven segment displays.
Designed for low pin-count embedded systems.

## Public API
- `shared_segment_buttons_init()`
- `shared_segment_buttons_update()`
- `shared_segment_buttons_is_pressed()`
- `shared_segment_buttons_get_key()`

## Shared IO Model
The library uses existing display wiring:
- shared segment lines
- digit select lines

Each button is mapped to a tuple:
- digit index
- segment line
- key code

## Non-Blocking Scanning Model
`shared_segment_buttons_update()` scans exactly one mapped button per call:
1. disable all digits briefly
2. set target segment as input
3. enable target digit select
4. sample shared line
5. disable digit and restore segment as output

No internal blocking delays are used.

## Multiplex Interaction
- Library is intended to run alongside `seven_segment_mux_update()`.
- Recommended call order in loop:
  1. `seven_segment_mux_update()`
  2. `shared_segment_buttons_update()`
- Short sampling window minimizes visible flicker.

## Configuration Model
`shared_segment_buttons_config_t`:
- `display` pointer (`seven_segment_mux_t`)
- `map` button mapping array
- `button_count`
- `active_level`
- `debounce_scans`

## Wiring Notes
- Button network must be designed for shared segment topology.
- Typical approach uses pull-up/pull-down with diode isolation as needed.
- Ensure electrical limits are respected for segment and digit lines.

## Limitations
- Single button sample per `update()` call.
- Debounce is scan-count based, not time-based.
- Wrong external wiring can cause ghost presses.

## Dependencies
- `core/*`
- `drivers/gpio/*`
- `libraries/display/seven_segment_mux/*`