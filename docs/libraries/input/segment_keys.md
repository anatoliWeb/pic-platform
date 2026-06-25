[Ukrainian version](./segment_keys.ua.md)

# Segment Keys Library

## Description

Shared-line key decoder for buttons connected to the segment bus of a seven-segment display.

## API

- `segment_keys_init()`
- `segment_keys_update()`
- `segment_keys_is_pressed()`
- `segment_keys_get_raw_mask()`
- `segment_keys_is_unknown()`
- `segment_keys_get_key()`

## Configuration

`segment_keys_config_t` contains:

- `display` - target `seven_segment_t`
- `keys` - table of exact segment masks
- `key_count`
- `active_level` - electrical active level seen during scan

Each `segment_keys_entry_t` contains:

- `segment_mask`
- `key`
- optional `button_t*`

## Scan Model

- All digit lines are disabled before sampling.
- Segment pins are temporarily switched to inputs.
- The library reads the exact segment mask currently asserted by the key wiring.
- Segment outputs are restored after sampling.
- `segment_keys` does not own display refresh. The application must keep calling `seven_segment_process()` or `seven_segment_refresh()` around scans.

## Button Integration

- `segment_keys` does not implement debounce, click, or hold by itself.
- If a key entry has `button != NULL`, the decoded pressed or released state is forwarded to `button_set_raw_state()` and `button_update()`.
- This keeps click, double-click, hold, and hold-repeat behavior inside the existing `button` library.
- Recommended integration path:
  - `button_init_external()`
  - `button_set_raw_state()`
  - `button_update()`

## Supported Key Wiring

- One key on one segment line
- One key encoded by multiple segment lines through diodes
- Exact-mask decoding only

## Example Families

- `examples-projects/xc8/seven_segment/keys_single_line.X`
- `examples-projects/xc8/seven_segment/keys_diode_coded.X`

## Example

```c
button_t key_button;
segment_keys_t keys;

button_init_external(&key_button, 1u);
segment_keys_init(&keys, &cfg);

while (1)
{
    segment_keys_update(&keys);

    if (button_clicked(&key_button))
    {
        /* handle click */
    }
}
```

## Notes

- Unknown or ambiguous masks are reported through `segment_keys_is_unknown()`.
- The library is intended for low pin-count display-plus-input designs.
- Electrical pull-up or pull-down network must match `active_level`.
- Exact-mask decoding keeps the library simple and predictable; overlapping masks should be handled in hardware design, not hidden inside the decoder.

## Status

- Library implemented.
- Example grouping completed.
- Manual Proteus validation is still pending.

## Dependencies

- `libraries/display/seven_segment`
- `libraries/input/button`
- `drivers/gpio/gpio.h`
