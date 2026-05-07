[🇺🇦 Ukrainian version](./ext_interrupt.ua.md)

# External Interrupt Driver

## Description
Driver for INT0/INT1/INT2 external interrupt control with callback dispatch model.

## API
- `ext_interrupt_init()`
- `ext_interrupt_enable()`
- `ext_interrupt_disable()`
- `ext_interrupt_set_edge()`
- `ext_interrupt_set_callback()`
- `ext_interrupt_clear_flag()`
- `ext_interrupt_get_flag()`
- `ext_interrupt_irq_handler()`

## Example
```c
ext_interrupt_init();
ext_interrupt_set_edge(EXT_INT0, EXT_INT_EDGE_FALLING);
ext_interrupt_set_callback(EXT_INT0, on_button_irq);
ext_interrupt_enable(EXT_INT0);
```

## Notes
- Real ISR is project-owned; call `ext_interrupt_irq_handler()` inside ISR.
- Callback model decouples driver from project logic.

## Dependencies
- `core/compiler.h`
- `core/types.h`
