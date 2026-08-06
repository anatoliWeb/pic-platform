# digital_input

## Purpose

Generic debounced digital input helper with active-high/low mapping, consuming edge flags, and optional latch.

## Location

```text
libraries/input/digital_input/digital_input.h
libraries/input/digital_input/digital_input.c
libraries/input/digital_input/example.c
docs/libraries/input/digital_input.md
docs/libraries/input/digital_input.ua.md
examples-projects/xc8/input/digital_input.X
scripts/tests/test_digital_input.py
```

## Use when

- you need reusable raw-sample debounce;
- you need active-high or active-low mapping;
- you need stable logical state and edge flags.

## Do not use when

- you need E-STOP, door, or product reaction policy;
- you want board pin reads inside the library;
- you need higher-level button semantics like click or hold.

## API

| Function/type | Purpose | What to pass | Output | Notes |
| --- | --- | --- | --- | --- |
| `digital_input_t` | caller-owned state | nothing | struct | no heap |
| `digital_input_config_t` | mapping and debounce | struct | config | validates in init |
| `digital_input_init()` | initialize state | input, config | `drv_status_t` | safe initial state |
| `digital_input_update()` | feed raw sample | input, raw level, `now_ms` | none | non-blocking |
| `digital_input_is_active()` | read logical state | input | `uint8_t` | non-consuming |
| `digital_input_rose()` | read rising edge | input | `uint8_t` | consuming |
| `digital_input_fell()` | read falling edge | input | `uint8_t` | consuming |
| `digital_input_is_latched()` | read latch state | input | `uint8_t` | non-consuming |
| `digital_input_clear_latch()` | clear latch | input | none | caller-owned |

## Dependencies

```text
core/compiler.h
core/types.h
```

## Runtime/time model

- Caller passes monotonic milliseconds.
- `debounce_ms` is the default stable-time threshold for both directions.
- `activate_debounce_ms` / `release_debounce_ms` override the symmetric debounce per direction. When both are 0, `debounce_ms` is used.
- `immediate_active` makes activation instant (0 ms debounce). Release still uses `release_debounce_ms`.
- `rose()` and `fell()` consume their flags.
- Latch stays set until `digital_input_clear_latch()`.

## ISR contract

- The module owns no ISR.
- Any scan loop or ISR-owned sampler may feed raw levels.

## Resource ownership

- No pin ownership.
- No board-specific action policy.
- No relay, fan, or heater logic.

## Examples

```text
examples-projects/xc8/input/digital_input.X
```

## Tests

```text
scripts/tests/test_digital_input.py
scripts/tests/test_examples_hierarchy.py
scripts/tests/test_check_agents_catalog.py
```

## Docs

```text
docs/libraries/input/digital_input.md
docs/libraries/input/digital_input.ua.md
```

## Limitations

- The helper works on raw levels only.
- Edge flags are single-consumer.

## AI decision rule

Use this module for generic digital input filtering. Keep application reaction outside it.
