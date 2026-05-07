[🇬🇧 English version](./portb_change.md)

# Драйвер PORTB Change

## Опис
Драйвер переривань по зміні RB4..RB7 з маскою змінених бітів та callback.

## API
- `portb_change_init()`
- `portb_change_enable()`
- `portb_change_disable()`
- `portb_change_read_port()`
- `portb_change_get_last_state()`
- `portb_change_get_changed_mask()`
- `portb_change_set_callback()`
- `portb_change_irq_handler()`

## Приклад
```c
portb_change_init();
portb_change_set_callback(on_rb_change);
portb_change_enable();
```

## Особливості
- ISR має викликати `portb_change_irq_handler()`.
- Callback отримує маску змін і поточний стан порту.

## Залежності
- `core/compiler.h`
- `core/types.h`
