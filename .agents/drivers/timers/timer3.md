# timer3

## Purpose

Timer3 backend driver.

## Location

```text
drivers/timers/timer3/timer3.h
drivers/timers/timer3/timer3.c
XC8/drivers/timers/timer3/timer3.c
C18/drivers/timers/timer3/timer3.c
```

## Use when

- you need reusable Timer3 control and callbacks.

## Do not use when

- Timer3 is already owned by another module.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `timer3_init` | `void timer3_init(uint16_t prescaler);` | initialize timer | prescaler | none | configures timer |  |
| `timer3_start` | `void timer3_start(void);` | start timer | none | none | starts counting |  |
| `timer3_stop` | `void timer3_stop(void);` | stop timer | none | none | stops counting |  |
| `timer3_set` | `void timer3_set(uint16_t value);` | set counter | value | none | updates counter |  |
| `timer3_get` | `uint16_t timer3_get(void);` | read counter | none | value | reads hardware |  |
| `timer3_enable_interrupt` | `void timer3_enable_interrupt(void);` | enable IRQ | none | none | sets interrupt enable |  |
| `timer3_disable_interrupt` | `void timer3_disable_interrupt(void);` | disable IRQ | none | none | clears interrupt enable |  |
| `timer3_set_callback` | `void timer3_set_callback(void (*cb)(void));` | install callback | callback | none | stores callback |  |
| `timer3_get_callback` | `void (*timer3_get_callback(void))(void);` | read callback | none | callback ptr | none |  |
| `timer3_irq_handler` | `void timer3_irq_handler(void);` | ISR dispatcher | none | none | invokes callback |  |

## Configuration structures

Not applicable for this module.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
drivers/timers/timer3/timer3.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/timers/timer3/timer3.c
```

### C18 implementation/wrapper

```text
C18/drivers/timers/timer3/timer3.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/timers/timer3/timer3.c

For C18 add:
C18/drivers/timers/timer3/timer3.c

Do not add together:
shared timer3.c and the selected wrapper .c

Reason:
the wrapper includes the shared implementation.
```

## Required files

```text
drivers/timers/timer3/timer3.h
drivers/timers/timer3/timer3.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
timer3_set_callback() callback from the application
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

Timer3 is a hardware backend with optional ISR callback dispatch.

## ISR requirements

Call `timer3_irq_handler()` from the timer interrupt.

## Resource ownership

Timer3 is project-owned when selected.

## Integration recipe

1. Add the shared Timer3 source or the selected wrapper.
2. Configure prescaler and load value.
3. Install a callback if needed.
4. Call the IRQ handler from the ISR.

## Minimal usable example

```c
timer3_init(8u);
timer3_start();
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

Timer3 conflicts with other Timer3-backed modules.

## Extension points

Keep scheduling logic in higher-level modules.

## Common mistakes

- Sharing Timer3 between unrelated modules.
- Compiling wrapper and shared source together.

## AI decision rule

Use this driver for reusable Timer3 backend control.
