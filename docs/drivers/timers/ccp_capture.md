[🇺🇦 Ukrainian version](./ccp_capture.ua.md)

# CCP Capture Driver

## Description
Capture-mode CCP driver for precise event timestamp sampling.

## API
- `ccp_capture_init()`
- `ccp_capture_enable()`
- `ccp_capture_disable()`
- `ccp_capture_get_value()`
- `ccp_capture_set_callback()`
- `ccp_capture_irq_handler()`

## Example
```c
ccp_capture_init(CCP_CAPTURE_RISING);
ccp_capture_set_callback(on_capture);
ccp_capture_enable();
```

## Notes
- ISR should call `ccp_capture_irq_handler()`.
- Typically uses Timer1 as time base.

## Dependencies
- `core/compiler.h`
- `core/types.h`
- Timer1 base
