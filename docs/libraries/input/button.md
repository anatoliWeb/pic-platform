[Ukrainian version](./button.ua.md)

# Button Driver

## Description

Non-blocking active-low button driver with debounce, click, double-click, hold, and hold-repeat support.

## API

- `button_init()`
- `button_init_external()`
- `button_set_raw_state()`
- `button_update()`
- `button_pressed()`
- `button_released()`
- `button_clicked()`
- `button_double_clicked()`
- `button_held()`
- `button_hold_repeated()`
- `button_get_click_count()`
- `button_is_clicked()`
- `button_is_held()`

## Example

```c
button_t btn;
button_init(&btn, &PORTB, &TRISB, 0u);

while (1)
{
    button_update(&btn);

    if (button_clicked(&btn))
    {
        /* handle click */
    }
}
```

## External-State Mode

Use `button_init_external()` when another library already decodes the electrical state and only needs button events.

```c
button_init_external(&btn, 1u);
button_set_raw_state(&btn, 0u);
button_update(&btn);
```

## Notes

- Active-low logic is used: `0 = pressed`, `1 = released`.
- Timing is based on `tick_get()`.
- External-state mode is used by shared-line input helpers such as `segment_keys`.
- `button` remains the single place for debounce and event logic even when the electrical scan is owned by another helper.
- This makes it safe to reuse the same click/hold behavior across direct GPIO buttons and shared-line button decoders.

## Related Examples

- `examples-projects/xc8/input/button.X`
- `examples-projects/xc8/seven_segment/keys_single_line.X`
- `examples-projects/xc8/seven_segment/keys_diode_coded.X`

## Dependencies

- `drivers/timers/tick`
