[🇺🇦 Ukrainian version](./button.ua.md)

# Button Driver

## Description

Non-blocking button driver with debounce and event flags.
Used for UI actions such as click and hold.

## API

- `button_init()`
- `button_update()`
- `button_is_clicked()`
- `button_is_held()`

## Example

```c
button_t btn;
button_init(&btn, &PORTB, &TRISB, 0u);
button_update(&btn);

if (button_is_clicked(&btn)) { /* click action */ }
if (button_is_held(&btn))    { /* hold action */ }
```

## Notes

- Debounce is handled internally with time-based state updates.
- Designed for periodic calls from scheduler/polling loop.

## Dependencies

- `core/compiler.h`
- `core/types.h`

