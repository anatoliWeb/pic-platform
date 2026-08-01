# shared_segment_buttons

## Purpose

Shared-line segment button scanner.

## Location

```text
libraries/input/shared_segment_buttons/shared_segment_buttons.h
libraries/input/shared_segment_buttons/shared_segment_buttons.c
```

## Use when

- you need button scanning over seven-segment segment lines.

## Do not use when

- the wiring does not match segment-bus buttons.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `SHARED_SEGMENT_BUTTONS_MAX` | macro | max entries | none | value | none |  |
| `shared_segment_id_t` | enum | segment id | `SHARED_SEG_SEG_A..DOT` | id | none |  |
| `shared_segment_button_map_t` | struct | map entry | segment/button mapping | state | none |  |
| `shared_segment_buttons_config_t` | struct | scanner config | display, maps, count | state | none |  |
| `shared_segment_buttons_t` | struct | scanner state | runtime state | state | none |  |
| `shared_segment_buttons_init` | `uint8_t shared_segment_buttons_init(...);` | initialize scanner | config | success flag | stores config | exact args in header |
| `shared_segment_buttons_update` | `void shared_segment_buttons_update(...);` | scan/update | scanner | none | reads display lines | exact args in header |
| `shared_segment_buttons_is_pressed` | `uint8_t shared_segment_buttons_is_pressed(...);` | pressed state | scanner, key | flag | none | exact args in header |
| `shared_segment_buttons_get_key` | `char shared_segment_buttons_get_key(...);` | decoded key | scanner | key | none | exact args in header |

## Configuration structures

Public fields are the config and state structs described in the header.

## Error/status model

`shared_segment_buttons_init()` returns a success flag.

## Source inclusion strategy

### Shared implementation

```text
libraries/input/shared_segment_buttons/shared_segment_buttons.c
```

### XC8 implementation/wrapper

```text
Not applicable for this module.
```

### C18 implementation/wrapper

```text
Not applicable for this module.
```

### Select exactly one route

```text
For XC8 add:
libraries/input/shared_segment_buttons/shared_segment_buttons.c

For C18 add:
libraries/input/shared_segment_buttons/shared_segment_buttons.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/input/shared_segment_buttons/shared_segment_buttons.h
libraries/input/shared_segment_buttons/shared_segment_buttons.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
libraries/display/seven_segment/seven_segment.h
```

### Callback-provided dependencies

Not applicable for this module.

### Optional feature dependencies

Not applicable for this module.

### Example-only dependencies

Not applicable for this module.

### Transitive dependencies

Not applicable for this module.

## Compile-time defines

Not applicable for this module.

## Runtime model

The scanner samples the shared segment bus while the display refresh continues.

## ISR requirements

Not applicable for this module.

## Resource ownership

Segment pins are shared with the display.

## Integration recipe

1. Add the shared scanner source.
2. Bind it to the display object.
3. Call update regularly.
4. Read decoded keys.

## Minimal usable example

```c
shared_segment_buttons_t buttons;
shared_segment_buttons_init(&buttons, &cfg);
shared_segment_buttons_update(&buttons);
```

## Working examples

Not applicable for this module.

## Proteus integration

Not applicable for this module.

## HEX artifacts

Not applicable for this module.

## Human documentation

Not applicable for this module.

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Exact-mask decoding only.

## Extension points

Keep display refresh logic outside the scanner.

## Common mistakes

- Stopping display refresh while scanning.
- Treating this as a generic keypad driver.

## AI decision rule

Use this module for shared segment-line button scanning.
