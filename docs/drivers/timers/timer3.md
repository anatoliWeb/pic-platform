[🇺🇦 Ukrainian version](./timer3.ua.md)

# Timer3 Driver

## Description
Timer3 low-level driver with interrupt/callback mechanism for timing tasks.

## API
- `timer3_init()`
- `timer3_start()`
- `timer3_stop()`
- `timer3_set()`
- `timer3_get()`
- `timer3_enable_interrupt()`
- `timer3_disable_interrupt()`
- `timer3_set_callback()`
- `timer3_irq_handler()`

## Example
```c
timer3_init(8u);
timer3_set_callback(on_tmr3_overflow);
timer3_enable_interrupt();
timer3_start();
```

## Notes
- `timer3_irq_handler()` must run from ISR path.
- Can be used as additional time base.

## Dependencies
- `core/compiler.h`
- `core/types.h`
