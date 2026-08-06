# cooldown_output

Reusable requested/active output helper with cooldown delay, remaining time, and optional callback.

## API

| Item | Notes |
| --- | --- |
| `cooldown_output_init()` | starts off, stores initial cooldown_ms |
| `cooldown_output_set_requested()` | request on/off with caller-supplied `now_ms` |
| `cooldown_output_set_duration_ms()` | change cooldown at runtime; recalculates deadline if cooling |
| `cooldown_output_cancel()` | cancel pending cooldown, preserve current active state |
| `cooldown_output_process()` | completes cooldown when the deadline expires |
| `cooldown_output_get_remaining_ms()` | wrap-safe remaining time query |
| `cooldown_output_force_off()` | cancel and force off immediately |

## Behavior

- `cooldown_ms == 0` means immediate off.
- Re-requesting on cancels pending shutdown.
- Callback fires only on active-state change.
- `set_duration_ms()` updates the stored cooldown. If currently cooling down, the deadline is recalculated from `now_ms` with the new duration. If the new duration is 0, the output turns off immediately.
- `cancel()` clears the cooling-down flag while preserving the current physical output state.
- For the wrap-safe deadline comparison to be valid, `cooldown_ms` must stay below `2^31` ms. The output must be polled via `cooldown_output_process()`.

## Examples

- `examples-projects/xc8/actuator/cooldown_output.X`

## Tests

- `scripts/tests/test_cooldown_output.py`
