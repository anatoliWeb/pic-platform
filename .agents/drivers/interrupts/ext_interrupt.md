# ext_interrupt

## Purpose

External interrupt driver.

## Location

```text
drivers/interrupts/ext_interrupt/ext_interrupt.h
drivers/interrupts/ext_interrupt/ext_interrupt.c
XC8/drivers/interrupts/ext_interrupt/ext_interrupt.c
C18/drivers/interrupts/ext_interrupt/ext_interrupt.c
```

## Use when

- you need reusable INT0/INT1/INT2 handling.

## Do not use when

- the project does not use external interrupts.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `ext_interrupt_channel_t` | enum | interrupt channel | `EXT_INT0`, `EXT_INT1`, `EXT_INT2` | channel | none |  |
| `ext_interrupt_edge_t` | enum | edge selection | `EXT_INT_EDGE_FALLING`, `EXT_INT_EDGE_RISING` | edge | none |  |
| `ext_interrupt_callback_t` | function pointer typedef | callback type | channel event | none | callback |  |
| `ext_interrupt_init` | `void ext_interrupt_init(void);` | initialize module | none | none | configures interrupt system |  |
| `ext_interrupt_enable` | `void ext_interrupt_enable(ext_interrupt_channel_t channel);` | enable channel | channel | none | sets interrupt enable |  |
| `ext_interrupt_disable` | `void ext_interrupt_disable(ext_interrupt_channel_t channel);` | disable channel | channel | none | clears interrupt enable |  |
| `ext_interrupt_set_edge` | `void ext_interrupt_set_edge(ext_interrupt_channel_t channel, ext_interrupt_edge_t edge);` | select edge | channel, edge | none | configures hardware edge |  |
| `ext_interrupt_set_callback` | `void ext_interrupt_set_callback(ext_interrupt_channel_t channel, ext_interrupt_callback_t callback);` | install callback | channel, callback | none | stores callback |  |
| `ext_interrupt_clear_flag` | `void ext_interrupt_clear_flag(ext_interrupt_channel_t channel);` | clear flag | channel | none | clears flag |  |
| `ext_interrupt_get_flag` | `uint8_t ext_interrupt_get_flag(ext_interrupt_channel_t channel);` | read flag | channel | flag | reads hardware flag |  |
| `ext_interrupt_irq_handler` | `void ext_interrupt_irq_handler(void);` | dispatch ISR | none | none | calls callbacks |  |

## Configuration structures

Not applicable for this module.

## Error/status model

Flags are boolean. Callbacks are optional.

## Source inclusion strategy

### Shared implementation

```text
drivers/interrupts/ext_interrupt/ext_interrupt.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/interrupts/ext_interrupt/ext_interrupt.c
```

### C18 implementation/wrapper

```text
C18/drivers/interrupts/ext_interrupt/ext_interrupt.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/interrupts/ext_interrupt/ext_interrupt.c

For C18 add:
C18/drivers/interrupts/ext_interrupt/ext_interrupt.c

Do not add together:
shared ext_interrupt.c and the selected wrapper .c

Reason:
the wrapper already compiles the shared implementation.
```

## Required files

```text
drivers/interrupts/ext_interrupt/ext_interrupt.h
drivers/interrupts/ext_interrupt/ext_interrupt.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
ext_interrupt_callback_t callbacks supplied by the application
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

Edge selection and callbacks are configured once, then the ISR dispatcher handles events.

## ISR requirements

Call `ext_interrupt_irq_handler()` from the global interrupt path.

## Resource ownership

INT0/INT1/INT2 pins are project-owned.

## Integration recipe

1. Add the shared external interrupt source or the selected wrapper.
2. Install callbacks and edge configuration.
3. Enable the selected channel.
4. Call the IRQ handler from the ISR.

## Minimal usable example

```c
ext_interrupt_init();
ext_interrupt_set_edge(EXT_INT0, EXT_INT_EDGE_FALLING);
ext_interrupt_enable(EXT_INT0);
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

Interrupt sources are hardware-specific.

## Extension points

Use callbacks instead of project-local ISR branches when possible.

## Common mistakes

- Forgetting to clear the flag.
- Mixing shared and wrapper sources.

## AI decision rule

Use this driver for reusable external interrupt handling.
