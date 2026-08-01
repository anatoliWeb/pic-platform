# timer0

## Purpose

Timer0 backend driver.

## Location

```text
drivers/timers/timer0/timer0.h
drivers/timers/timer0/timer0.c
XC8/drivers/timers/timer0/timer0.c
C18/drivers/timers/timer0/timer0.c
```

## Use when

- you need reusable Timer0 control and callbacks.

## Do not use when

- another timer backend is already owned by the project.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `timer0_init` | `void timer0_init(uint16_t prescaler);` | initialize timer | prescaler | none | configures timer |  |
| `timer0_start` | `void timer0_start(void);` | start timer | none | none | starts counting |  |
| `timer0_stop` | `void timer0_stop(void);` | stop timer | none | none | stops counting |  |
| `timer0_set` | `void timer0_set(uint16_t value);` | set counter | value | none | updates counter |  |
| `timer0_get` | `uint16_t timer0_get(void);` | read counter | none | value | reads hardware |  |
| `timer0_enable_interrupt` | `void timer0_enable_interrupt(void);` | enable IRQ | none | none | sets interrupt enable |  |
| `timer0_disable_interrupt` | `void timer0_disable_interrupt(void);` | disable IRQ | none | none | clears interrupt enable |  |
| `timer0_set_callback` | `void timer0_set_callback(void (*cb)(void));` | install callback | callback | none | stores callback |  |
| `timer0_get_callback` | `void (*timer0_get_callback(void))(void);` | read callback | none | callback ptr | none |  |
| `timer0_irq_handler` | `void timer0_irq_handler(void);` | ISR dispatcher | none | none | invokes callback |  |

## Configuration structures

Not applicable for this module.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
drivers/timers/timer0/timer0.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/timers/timer0/timer0.c
```

### C18 implementation/wrapper

```text
C18/drivers/timers/timer0/timer0.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/timers/timer0/timer0.c

For C18 add:
C18/drivers/timers/timer0/timer0.c

Do not add together:
shared timer0.c and the selected wrapper .c

Reason:
the wrapper includes the shared implementation.
```

## Required files

```text
drivers/timers/timer0/timer0.h
drivers/timers/timer0/timer0.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
timer0_set_callback() callback from the application
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

Timer0 is a hardware backend with optional ISR callback dispatch.

## ISR requirements

Call `timer0_irq_handler()` from the timer interrupt.

## Resource ownership

Timer0 is project-owned when selected.

## Integration recipe

1. Add the shared Timer0 source or the selected wrapper.
2. Configure prescaler and load value.
3. Install a callback if needed.
4. Call the IRQ handler from the ISR.

## Minimal usable example

```c
timer0_init(8u);
timer0_start();
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

Timer0 ownership conflicts with any other backend using Timer0.

## Extension points

Keep scheduling logic in higher-level modules.

## Common mistakes

- Sharing Timer0 between unrelated modules.
- Compiling wrapper and shared source together.

## AI decision rule

Use this driver for reusable Timer0 backend control.
