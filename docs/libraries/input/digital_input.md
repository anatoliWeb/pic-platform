# digital_input

Reusable debounced digital input helper with active-high/low mapping, consuming edge flags, and optional asymmetric debounce.

## API

| Item | Notes |
| --- | --- |
| `digital_input_init()` | validates config and seeds a safe initial state |
| `digital_input_update()` | feeds raw levels and advances debounce |
| `digital_input_rose()` / `digital_input_fell()` | consuming edge flags |
| `digital_input_is_latched()` | read-only latch query |

## Behavior

- `active_level` selects active-high or active-low mapping.
- `debounce_ms` is stable-time based, used as the default for both directions.
- `activate_debounce_ms` and `release_debounce_ms` override the symmetric debounce per direction. When both are 0, `debounce_ms` is used (backward compatible).
- `immediate_active` makes activation instant (0 ms debounce). Release still uses `release_debounce_ms`.
- Latch is optional and clears only by `digital_input_clear_latch()`.

## Examples

- `examples-projects/xc8/input/digital_input.X`

## Tests

- `scripts/tests/test_digital_input.py`
