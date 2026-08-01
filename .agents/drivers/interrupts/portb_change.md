# portb_change

## Purpose

PORTB change interrupt driver.

## Location

```text
drivers/interrupts/portb_change/portb_change.h
drivers/interrupts/portb_change/portb_change.c
XC8/drivers/interrupts/portb_change/portb_change.c
C18/drivers/interrupts/portb_change/portb_change.c
```

## Use when

- you need reusable PORTB change interrupt handling.

## Do not use when

- the project does not use PORTB change interrupts.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `portb_change_callback_t` | function pointer typedef | callback type | port change event | none | callback |  |
| `portb_change_init` | `void portb_change_init(void);` | initialize module | none | none | configures hardware |  |
| `portb_change_enable` | `void portb_change_enable(void);` | enable interrupt | none | none | sets enable bit |  |
| `portb_change_disable` | `void portb_change_disable(void);` | disable interrupt | none | none | clears enable bit |  |
| `portb_change_read_port` | `uint8_t portb_change_read_port(void);` | read current port | none | port state | reads hardware |  |
| `portb_change_get_last_state` | `uint8_t portb_change_get_last_state(void);` | read latched state | none | last state | none |  |
| `portb_change_get_changed_mask` | `uint8_t portb_change_get_changed_mask(void);` | changed bits | none | mask | none |  |
| `portb_change_set_callback` | `void portb_change_set_callback(portb_change_callback_t cb);` | install callback | callback | none | stores callback |  |
| `portb_change_irq_handler` | `void portb_change_irq_handler(void);` | ISR dispatcher | none | none | invokes callback |  |

## Configuration structures

Not applicable for this module.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
drivers/interrupts/portb_change/portb_change.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/interrupts/portb_change/portb_change.c
```

### C18 implementation/wrapper

```text
C18/drivers/interrupts/portb_change/portb_change.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/interrupts/portb_change/portb_change.c

For C18 add:
C18/drivers/interrupts/portb_change/portb_change.c

Do not add together:
shared portb_change.c and the selected wrapper .c

Reason:
the wrapper includes the shared implementation.
```

## Required files

```text
drivers/interrupts/portb_change/portb_change.h
drivers/interrupts/portb_change/portb_change.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
portb_change_callback_t callbacks supplied by the application
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

The module tracks last PORTB state and computes changed bits inside the ISR path.

## ISR requirements

Call `portb_change_irq_handler()` from the global interrupt path.

## Resource ownership

PORTB change pins are project-owned.

## Integration recipe

1. Add the shared PORTB change source or the selected wrapper.
2. Install the callback if needed.
3. Enable the interrupt.
4. Call the IRQ handler from the ISR.

## Minimal usable example

```c
portb_change_init();
portb_change_enable();
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

Only PORTB change handling is covered.

## Extension points

Keep edge decoding in application callbacks if extra semantics are needed.

## Common mistakes

- Forgetting to clear the hardware state mismatch.
- Compiling wrapper and shared source together.

## AI decision rule

Use this driver for reusable PORTB change interrupt handling.
