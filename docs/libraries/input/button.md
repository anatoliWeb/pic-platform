[🇺🇦 Ukrainian version](./button.ua.md)

# Button Driver

## Description

Phase 1 non-blocking button driver with debounce. It is intended for stand-alone input examples. Phase 2 menu navigation layers will build on top of this library, but they are not part of it.

## API

- `button_init()`
- `button_update()`
- `button_pressed()`
- `button_released()`
- `button_clicked()`
- `button_double_clicked()`
- `button_held()`
- `button_hold_repeated()`
- `button_get_click_count()`
- `button_is_clicked()` - compatibility wrapper
- `button_is_held()` - compatibility wrapper

## Example

```c
button_t btn;
button_init(&btn, &PORTB, &TRISB, 0u);

while (1)
{
    button_update(&btn);

    if (button_pressed(&btn)) { /* press action */ }
    if (button_released(&btn)) { /* release action */ }
    if (button_clicked(&btn)) { /* click action */ }
    if (button_double_clicked(&btn)) { /* double click action */ }
    if (button_held(&btn)) { /* hold action */ }
    if (button_hold_repeated(&btn)) { /* repeat action */ }
}
```

## Notes

- Active-low input is expected.
- Debounce uses polling and `tick_get()`.
- Hold repeat and click count are lightweight and static.
- `button_is_clicked()` and `button_is_held()` remain available for older code.
- Phase 2 menu/navigation layers are not part of this library.

## Dependencies

- `core/compiler.h`
- `core/types.h`
- `drivers/timers/tick`