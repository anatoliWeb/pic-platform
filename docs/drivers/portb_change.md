[🇺🇦 Ukrainian version](./portb_change.ua.md)

# PORTB Change Driver

## Description
RB4..RB7 change-interrupt driver with changed-mask reporting and callback hook.

## API
- `portb_change_init()`
- `portb_change_enable()`
- `portb_change_disable()`
- `portb_change_read_port()`
- `portb_change_get_last_state()`
- `portb_change_get_changed_mask()`
- `portb_change_set_callback()`
- `portb_change_irq_handler()`

## Example
```c
portb_change_init();
portb_change_set_callback(on_rb_change);
portb_change_enable();
```

## Notes
- ISR must call `portb_change_irq_handler()`.
- Callback receives changed mask and current state.

## Dependencies
- `core/compiler.h`
- `core/types.h`
