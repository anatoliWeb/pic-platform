# timer1

## Purpose

Timer1 backend driver.

## Location

```text
drivers/timers/timer1/timer1.h
drivers/timers/timer1/timer1.c
XC8/drivers/timers/timer1/timer1.c
C18/drivers/timers/timer1/timer1.c
```

## Use when

- you need reusable Timer1 control and callbacks.

## Do not use when

- Timer1 is already owned by tick or another module.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `timer1_init` | `void timer1_init(uint16_t prescaler);` | initialize timer | prescaler | none | configures timer |  |
| `timer1_start` | `void timer1_start(void);` | start timer | none | none | starts counting |  |
| `timer1_stop` | `void timer1_stop(void);` | stop timer | none | none | stops counting |  |
| `timer1_set` | `void timer1_set(uint16_t value);` | set counter | value | none | updates counter |  |
| `timer1_get` | `uint16_t timer1_get(void);` | read counter | none | value | reads hardware |  |
| `timer1_enable_interrupt` | `void timer1_enable_interrupt(void);` | enable IRQ | none | none | sets interrupt enable |  |
| `timer1_disable_interrupt` | `void timer1_disable_interrupt(void);` | disable IRQ | none | none | clears interrupt enable |  |
| `timer1_set_callback` | `void timer1_set_callback(void (*cb)(void));` | install callback | callback | none | stores callback |  |
| `timer1_get_callback` | `void (*timer1_get_callback(void))(void);` | read callback | none | callback ptr | none |  |
| `timer1_irq_handler` | `void timer1_irq_handler(void);` | ISR dispatcher | none | none | invokes callback |  |

## Configuration structures

Not applicable for this module.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
drivers/timers/timer1/timer1.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/timers/timer1/timer1.c
```

### C18 implementation/wrapper

```text
C18/drivers/timers/timer1/timer1.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/timers/timer1/timer1.c

For C18 add:
C18/drivers/timers/timer1/timer1.c

Do not add together:
drivers/timers/timer1/timer1.c and the selected compiler-specific .c

Reason:
the shared source dispatches to the compiler-specific implementation.
```

## Required files

```text
drivers/timers/timer1/timer1.h
drivers/timers/timer1/timer1.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
timer1_set_callback() callback from the application
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

Timer1 is a hardware backend with optional ISR callback dispatch.

## ISR requirements

Call `timer1_irq_handler()` from the timer interrupt.

## Resource ownership

Timer1 is project-owned when selected.

## Integration recipe

1. Add the shared Timer1 source or the selected wrapper.
2. Configure prescaler and load value.
3. Install a callback if needed.
4. Call the IRQ handler from the ISR.

## Minimal usable example

```c
timer1_init(8u);
timer1_start();
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

Timer1 conflicts with tick ownership.

## Extension points

Keep scheduling logic in higher-level modules.

## Common mistakes

- Sharing Timer1 between unrelated modules.
- Compiling wrapper and shared source together.

## AI decision rule

Use this driver for reusable Timer1 backend control.
