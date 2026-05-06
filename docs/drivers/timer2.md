[🇺🇦 Ukrainian version](./timer2.ua.md)

# Timer2 Driver

## Description
Timer2 low-level driver with periodic timing support and interrupt callback flow.

## API
- `timer2_init()`
- `timer2_start()`
- `timer2_stop()`
- `timer2_set()`
- `timer2_get()`
- `timer2_enable_interrupt()`
- `timer2_disable_interrupt()`
- `timer2_set_callback()`
- `timer2_irq_handler()`

## Example
```c
timer2_init(16u);
timer2_set_callback(on_tmr2_overflow);
timer2_enable_interrupt();
timer2_start();
```

## Notes
- `timer2_irq_handler()` is ISR hook.
- Often used together with PWM timing paths.

## Dependencies
- `core/compiler.h`
- `core/types.h`
