# cooldown_output

## Purpose

Reusable requested/active output helper with cooldown delay, re-request cancelation, and optional callback.

## Location

```text
libraries/actuator/cooldown_output/cooldown_output.h
libraries/actuator/cooldown_output/cooldown_output.c
libraries/actuator/cooldown_output/example.c
docs/libraries/actuator/cooldown_output.md
docs/libraries/actuator/cooldown_output.ua.md
examples-projects/xc8/actuator/cooldown_output.X
scripts/tests/test_cooldown_output.py
```

## Use when

- you need a requested state and a delayed shutdown;
- you need remaining cooldown time;
- you want an optional state-change callback without pin binding.

## Do not use when

- you need fan, heater, or safety policy inside the library;
- you want direct GPIO or relay pin ownership;
- you need a different actuator model.

## API

| Function/type | Purpose | What to pass | Output | Notes |
| --- | --- | --- | --- | --- |
| `cooldown_output_t` | caller-owned state | nothing | struct | no heap |
| `cooldown_output_config_t` | cooldown and callback | struct | config | validates in init |
| `cooldown_output_init()` | initialize state | output, config | `drv_status_t` | starts off |
| `cooldown_output_set_requested()` | request on/off | output, requested, `now_ms` | none | off starts cooldown |
| `cooldown_output_process()` | advance cooldown | output, `now_ms` | none | non-blocking |
| `cooldown_output_is_active()` | read active state | output | `uint8_t` | non-consuming |
| `cooldown_output_is_cooling_down()` | read cooldown state | output | `uint8_t` | non-consuming |
| `cooldown_output_get_remaining_ms()` | read remaining cooldown | output, `now_ms` | `uint32_t` | wrap-safe |
| `cooldown_output_force_off()` | cancel and force off | output | none | state-change callback allowed |

## Dependencies

```text
core/compiler.h
core/types.h
```

## Runtime/time model

- Caller passes monotonic milliseconds.
- `cooldown_ms == 0` means immediate off.
- Re-requesting on cancels pending shutdown.
- The callback fires only on active-state changes.

## ISR contract

- The module owns no ISR.
- An application loop or timer task calls `cooldown_output_process()`.

## Resource ownership

- No pin ownership.
- No relay or fan policy.
- Output side effects are only via the optional callback.

## Examples

```text
examples-projects/xc8/actuator/cooldown_output.X
```

## Tests

```text
scripts/tests/test_cooldown_output.py
scripts/tests/test_examples_hierarchy.py
scripts/tests/test_check_agents_catalog.py
```

## Docs

```text
docs/libraries/actuator/cooldown_output.md
docs/libraries/actuator/cooldown_output.ua.md
```

## Limitations

- The helper does not own hardware outputs.
- `get_remaining_ms()` is based on caller-supplied time.
- `cooldown_ms` must stay below `2^31` ms for the wrap-safe deadline comparison.
- `cooldown_output_process()` must be polled for a cooldown to expire.

## AI decision rule

Use this module for reusable cooldown output logic only. Keep product policy elsewhere.
