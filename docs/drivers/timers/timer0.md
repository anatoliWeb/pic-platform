[🇺🇦 Ukrainian version](./timer0.ua.md)

# Timer0 Driver

## Description
Timer0 low-level driver for counter/timing tasks with optional overflow interrupt callback.

## API
- `timer0_init()`
- `timer0_start()`
- `timer0_stop()`
- `timer0_set()`
- `timer0_get()`
- `timer0_enable_interrupt()`
- `timer0_disable_interrupt()`
- `timer0_set_callback()`
- `timer0_irq_handler()`

## Example
```c
timer0_init(256u);
timer0_set_callback(on_tmr0_overflow);
timer0_enable_interrupt();
timer0_start();
```

## Notes
- Use `timer0_irq_handler()` inside MCU ISR.
- Can be used as base for periodic tick logic.

## Dependencies
- `core/compiler.h`
- `core/types.h`
