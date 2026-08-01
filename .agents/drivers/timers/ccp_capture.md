# ccp_capture

## Purpose

CCP capture mode driver.

## Location

```text
drivers/timers/ccp_capture/ccp_capture.h
drivers/timers/ccp_capture/ccp_capture.c
XC8/drivers/timers/ccp_capture/ccp_capture.c
C18/drivers/timers/ccp_capture/ccp_capture.c
```

## Use when

- you need reusable capture-event handling.

## Do not use when

- capture mode is not part of the project.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `ccp_capture_mode_t` | enum | capture mode | `CCP_CAPTURE_RISING`, `CCP_CAPTURE_FALLING`, `CCP_CAPTURE_EVERY_4`, `CCP_CAPTURE_EVERY_16` | mode | none |  |
| `ccp_capture_callback_t` | function pointer typedef | callback type | capture event | none | callback |  |
| `ccp_capture_init` | `void ccp_capture_init(ccp_capture_mode_t mode);` | initialize module | mode | none | configures hardware |  |
| `ccp_capture_enable` | `void ccp_capture_enable(void);` | enable capture | none | none | sets enable bit |  |
| `ccp_capture_disable` | `void ccp_capture_disable(void);` | disable capture | none | none | clears enable bit |  |
| `ccp_capture_get_value` | `uint16_t ccp_capture_get_value(void);` | read captured value | none | value | reads hardware |  |
| `ccp_capture_set_callback` | `void ccp_capture_set_callback(ccp_capture_callback_t cb);` | install callback | callback | none | stores callback |  |
| `ccp_capture_irq_handler` | `void ccp_capture_irq_handler(void);` | ISR dispatcher | none | none | invokes callback |  |

## Configuration structures

Not applicable for this module.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
drivers/timers/ccp_capture/ccp_capture.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/timers/ccp_capture/ccp_capture.c
```

### C18 implementation/wrapper

```text
C18/drivers/timers/ccp_capture/ccp_capture.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/timers/ccp_capture/ccp_capture.c

For C18 add:
C18/drivers/timers/ccp_capture/ccp_capture.c

Do not add together:
shared ccp_capture.c and the selected wrapper .c

Reason:
the wrapper includes the shared implementation.
```

## Required files

```text
drivers/timers/ccp_capture/ccp_capture.h
drivers/timers/ccp_capture/ccp_capture.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
ccp_capture_callback_t callback from the application
```

### Optional feature dependencies

Not applicable for this module.

### Example-only dependencies

Not applicable for this module.

### Transitive dependencies

Not applicable for this module.

## Compile-time defines

Not applicable for this module.

## Runtime model

Capture events are delivered via the ISR dispatcher.

## ISR requirements

Call `ccp_capture_irq_handler()` from the capture interrupt.

## Resource ownership

CCP capture peripheral ownership is project-specific.

## Integration recipe

1. Add the shared CCP capture source or the selected wrapper.
2. Set capture mode and callback.
3. Enable the module.
4. Call the IRQ handler from the ISR.

## Minimal usable example

```c
ccp_capture_init(CCP_CAPTURE_RISING);
ccp_capture_enable();
```

## Working examples

Not applicable for this module.

## Proteus integration

Not applicable for this module.

## HEX artifacts

Not applicable for this module.

## Human documentation

Not applicable for this module.

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Capture channel ownership is hardware-specific.

## Extension points

Keep capture post-processing in higher-level reusable code.

## Common mistakes

- Forgetting to call the IRQ handler.
- Compiling wrapper and shared source together.

## AI decision rule

Use this driver for reusable capture-event handling.
