# seven_segment_mux

## Purpose

Seven-segment multiplexing helper.

## Location

```text
libraries/display/seven_segment_mux/seven_segment_mux.h
libraries/display/seven_segment_mux/seven_segment_mux.c
XC8/libraries/display/seven_segment_mux/seven_segment_mux.c
C18/libraries/display/seven_segment_mux/seven_segment_mux.c
```

## Use when

- you need multiplexed seven-segment display support.

## Do not use when

- a simple single-instance `seven_segment` display is enough.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `SEVEN_SEGMENT_MUX_MAX_DIGITS` | macro | max digits | none | value | none |  |
| `SEVEN_SEGMENT_MUX_CHAR_MINUS` | macro | minus symbol | none | value | none |  |
| `SEVEN_SEGMENT_MUX_CHAR_BLANK` | macro | blank symbol | none | value | none |  |
| `seven_segment_mux_config_t` | struct | mux config | digit pins, segments, timer, count | state | none |  |
| `seven_segment_mux_t` | struct | mux state | buffer, brightness, timers | state | none |  |
| `seven_segment_mux_init` | `uint8_t seven_segment_mux_init(...);` | initialize mux | config | success flag | configures pins | exact args in header |
| `seven_segment_mux_set_digit` | `void seven_segment_mux_set_digit(...);` | set digit | display, index, value | none | updates buffer | exact args in header |
| `seven_segment_mux_set_number` | `void seven_segment_mux_set_number(...);` | set number | display, value | none | updates buffer | exact args in header |
| `seven_segment_mux_set_hex` | `void seven_segment_mux_set_hex(...);` | set hex | display, value | none | updates buffer | exact args in header |
| `seven_segment_mux_clear` | `void seven_segment_mux_clear(...);` | clear display | display | none | clears buffer | exact args in header |
| `seven_segment_mux_set_dot` | `void seven_segment_mux_set_dot(...);` | dot control | display, digit, flag | none | updates buffer | exact args in header |
| `seven_segment_mux_update` | `void seven_segment_mux_update(...);` | refresh step | display | none | toggles digits | exact args in header |

## Configuration structures

Public fields are the config and state structs described in the header.

## Error/status model

`seven_segment_mux_init()` returns a success flag.

## Source inclusion strategy

### Shared implementation

```text
libraries/display/seven_segment_mux/seven_segment_mux.c
```

### XC8 implementation/wrapper

```text
XC8/libraries/display/seven_segment_mux/seven_segment_mux.c
```

### C18 implementation/wrapper

```text
C18/libraries/display/seven_segment_mux/seven_segment_mux.c
```

### Select exactly one route

```text
For XC8 add:
XC8/libraries/display/seven_segment_mux/seven_segment_mux.c

For C18 add:
C18/libraries/display/seven_segment_mux/seven_segment_mux.c

Do not add together:
shared seven_segment_mux.c and the selected wrapper .c

Reason:
the wrapper includes the shared implementation.
```

## Required files

```text
libraries/display/seven_segment_mux/seven_segment_mux.h
libraries/display/seven_segment_mux/seven_segment_mux.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
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

The module multiplexes digits through periodic update calls.

## ISR requirements

The update step is typically called from a timer-driven path.

## Resource ownership

Digit and segment pins are project-owned.

## Integration recipe

1. Add the shared mux source or selected wrapper.
2. Configure digit and segment pins.
3. Initialize the mux.
4. Call `seven_segment_mux_update()` from a timer path.

## Minimal usable example

```c
seven_segment_mux_init(&mux, &cfg);
seven_segment_mux_set_number(&mux, 1234);
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

Timer ownership must be managed by the caller.

## Extension points

Keep display formatting logic in higher-level helpers.

## Common mistakes

- Calling update too slowly.
- Compiling wrapper and shared source together.

## AI decision rule

Use this helper for multiplexed seven-segment displays.
