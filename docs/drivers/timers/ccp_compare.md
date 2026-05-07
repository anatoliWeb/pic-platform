[🇺🇦 Ukrainian version](./ccp_compare.ua.md)

# CCP Compare Driver

## Description
Compare-mode CCP driver for timed match events.

## API
- `ccp_compare_init()`
- `ccp_compare_enable()`
- `ccp_compare_disable()`
- `ccp_compare_set_value()`
- `ccp_compare_set_callback()`
- `ccp_compare_irq_handler()`

## Example
```c
ccp_compare_init();
ccp_compare_set_value(50000u);
ccp_compare_set_callback(on_compare);
ccp_compare_enable();
```

## Notes
- ISR should call `ccp_compare_irq_handler()`.
- Used for deterministic timed events.

## Dependencies
- `core/compiler.h`
- `core/types.h`
- Timer base
