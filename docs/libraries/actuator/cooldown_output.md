# cooldown_output

Reusable requested/active output helper with cooldown delay, remaining time, and optional callback.

## API

| Item | Notes |
| --- | --- |
| `cooldown_output_init()` | starts off |
| `cooldown_output_set_requested()` | request on/off with caller-supplied `now_ms` |
| `cooldown_output_process()` | completes cooldown when the deadline expires |
| `cooldown_output_get_remaining_ms()` | wrap-safe remaining time query |

## Behavior

- `cooldown_ms == 0` means immediate off.
- Re-requesting on cancels pending shutdown.
- Callback fires only on active-state change.

## Examples

- `examples-projects/xc8/actuator/cooldown_output.X`

## Tests

- `scripts/tests/test_cooldown_output.py`
