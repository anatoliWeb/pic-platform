# segment_keys

## Purpose

Shared-line key decoder for buttons connected to the segment bus of a seven-segment display.

## Location

```text
libraries/input/segment_keys/segment_keys.h
libraries/input/segment_keys/segment_keys.c
XC8/libraries/input/segment_keys/segment_keys.c
C18/libraries/input/segment_keys/segment_keys.c
docs/libraries/input/segment_keys.md
docs/libraries/input/segment_keys.ua.md
examples-projects/xc8/seven_segment/keys_single_line.X
examples-projects/xc8/seven_segment/keys_diode_coded.X
```

## Use when

- you need buttons on seven-segment segment lines;
- you need exact-mask decoding of shared bus keys;
- you want to reuse `button` for events without duplicating the scanner.

## Do not use when

- you need a generic keypad matrix scanner;
- the wiring does not match exact segment masks;
- a new project-local scanner would duplicate the same behavior.

## Public API

| Function/type | Purpose | What to pass | Returns / output | Notes |
| --- | --- | --- | --- | --- |
| `segment_keys_entry_t` | one decoded key | mask, key, optional button | struct | optional `button_t*` |
| `segment_keys_config_t` | key scanner configuration | display, key table, count, active level | struct | binds to `seven_segment_t` |
| `segment_keys_t` | scanner state | nothing | struct | stores last raw mask/key |
| `segment_keys_init()` | initialize decoder | context, config | `uint8_t` | returns success flag |
| `segment_keys_update()` | sample the bus | context | none | call while display refresh stays active |
| `segment_keys_is_pressed()` | key press state | context, key index | `uint8_t` | for decoded buttons |
| `segment_keys_get_raw_mask()` | current raw segment mask | context | `uint8_t` | diagnostics |
| `segment_keys_is_unknown()` | unknown/ambiguous state | context | `uint8_t` | diagnostics |
| `segment_keys_get_key()` | last decoded key | context | `char` | returns last key symbol |

## Required files

### Core/library files

```text
libraries/input/segment_keys/segment_keys.h
libraries/input/segment_keys/segment_keys.c
```

### XC8 wrapper files

```text
XC8/libraries/input/segment_keys/segment_keys.c
```

### C18 wrapper files

```text
C18/libraries/input/segment_keys/segment_keys.c
```

## Dependencies

```text
libraries/display/seven_segment/seven_segment.h
libraries/input/button/button.h
drivers/gpio/gpio.h
```

## Configuration defines

| Define | Default | Where | Effect | Code size impact |
| --- | ---: | --- | --- | --- |
| none module-specific | n/a | config struct | key table and active level are runtime config | none |

## Runtime model

- The decoder reads the current segment bus mask from the display path.
- Exact raw masks map to keys.
- If a matching entry contains `button != NULL`, the decoded state is forwarded to `button_set_raw_state()` and `button_update()`.
- The display refresh must continue while scanning.

## Integration recipe

1. Add `seven_segment`, `button`, `segment_keys`, and GPIO files.
2. Keep display refresh active.
3. Fill the key table with exact masks and optional `button_t` entries.
4. Call `segment_keys_update()` regularly.
5. Read `segment_keys_get_key()` or the linked button events.

## Minimal usage example

```c
segment_keys_t keys;

segment_keys_init(&keys, &cfg);

while (1)
{
    segment_keys_update(&keys);
}
```

## Resource ownership

- Segment pins are shared with the display.
- `seven_segment` must keep refreshing.
- Button semantics stay in `button`.

## Examples

```text
examples-projects/xc8/seven_segment/keys_single_line.X
examples-projects/xc8/seven_segment/keys_diode_coded.X
```

## Human documentation

```text
docs/libraries/input/segment_keys.md
docs/libraries/input/segment_keys.ua.md
```

## Known limitations

- Exact-mask decoding only.
- No built-in debounce or hold logic.

## Extension points

- Extend the key map or forward more entries into `button` instead of writing a project-local scanner.

## AI decision rule

Do not create a project-local segment-key scanner if `segment_keys` already fits. Extend `segment_keys` or app-level mapping only if needed.
