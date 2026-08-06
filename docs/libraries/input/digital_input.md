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
- `immediate_active` is checked FIRST: when set, activation is instant (0 ms) regardless of directional fields. This is the priority order:
  1. `immediate_active` → activation instant
  2. `activate_debounce_ms` / `release_debounce_ms` → directional thresholds
  3. `debounce_ms` → symmetric fallback (when both directional fields are 0)
- `activate_debounce_ms` and `release_debounce_ms` override the symmetric debounce per direction. When both are 0, `debounce_ms` is used (backward compatible).

### Debounce modes

**Legacy symmetric mode** (both directional fields = 0):
- Both directions use `debounce_ms`.
- Exception: if `immediate_active` is set, activation is still instant.
- This is the backward-compatible default for existing configs.

**Asymmetric mode** (at least one directional field != 0):
- Each direction uses its own threshold.
- `activate_debounce_ms == 0` means immediate activation.
- `release_debounce_ms == 0` means immediate release.
- Nonzero value means stable-time debounce for that direction.
- `immediate_active` still overrides activation to 0 ms.

- Latch is optional and clears only by `digital_input_clear_latch()`.

## Examples

- `examples-projects/xc8/input/digital_input.X`

## Tests

- `scripts/tests/test_digital_input.py`
