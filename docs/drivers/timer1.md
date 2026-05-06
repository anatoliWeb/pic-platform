[🇺🇦 Ukrainian version](./timer1.ua.md)

# Timer1 Driver

## Description
Timer1 low-level driver with start/stop, value read/write, interrupt control, and callback hook.

## API
- `timer1_init()`
- `timer1_start()`
- `timer1_stop()`
- `timer1_set()`
- `timer1_get()`
- `timer1_enable_interrupt()`
- `timer1_disable_interrupt()`
- `timer1_set_callback()`
- `timer1_irq_handler()`

## Example
```c
timer1_init(8u);
timer1_set_callback(on_tmr1_overflow);
timer1_enable_interrupt();
timer1_start();
```

## Notes
- `timer1_irq_handler()` must be called from ISR.
- Commonly used for periodic tick and time base.

## Dependencies
- `core/compiler.h`
- `core/types.h`
