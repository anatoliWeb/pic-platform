# timer2

## Purpose

Timer2 backend driver.

## Location

```text
drivers/timers/timer2/timer2.h
drivers/timers/timer2/timer2.c
XC8/drivers/timers/timer2/timer2.c
C18/drivers/timers/timer2/timer2.c
```

## Use when

- you need reusable Timer2 control and callbacks.

## Do not use when

- Timer2 is already owned by another module.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `timer2_init` | `void timer2_init(uint16_t prescaler);` | initialize timer | prescaler | none | configures timer |  |
| `timer2_start` | `void timer2_start(void);` | start timer | none | none | starts counting |  |
| `timer2_stop` | `void timer2_stop(void);` | stop timer | none | none | stops counting |  |
| `timer2_set` | `void timer2_set(uint16_t value);` | set counter | value | none | updates counter |  |
| `timer2_get` | `uint16_t timer2_get(void);` | read counter | none | value | reads hardware |  |
| `timer2_enable_interrupt` | `void timer2_enable_interrupt(void);` | enable IRQ | none | none | sets interrupt enable |  |
| `timer2_disable_interrupt` | `void timer2_disable_interrupt(void);` | disable IRQ | none | none | clears interrupt enable |  |
| `timer2_clear_interrupt_flag` | `void timer2_clear_interrupt_flag(void);` | clear flag | none | none | clears flag |  |
| `timer2_set_callback` | `void timer2_set_callback(void (*cb)(void));` | install callback | callback | none | stores callback |  |
| `timer2_get_callback` | `void (*timer2_get_callback(void))(void);` | read callback | none | callback ptr | none |  |
| `timer2_set_period` | `void timer2_set_period(uint8_t value);` | set period | value | none | updates period |  |
| `timer2_set_postscaler` | `void timer2_set_postscaler(uint8_t value);` | set postscaler | value | none | updates hardware |  |
| `timer2_irq_handler` | `void timer2_irq_handler(void);` | ISR dispatcher | none | none | invokes callback |  |

## Configuration structures

Not applicable for this module.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
drivers/timers/timer2/timer2.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/timers/timer2/timer2.c
```

### C18 implementation/wrapper

```text
C18/drivers/timers/timer2/timer2.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/timers/timer2/timer2.c

For C18 add:
C18/drivers/timers/timer2/timer2.c

Do not add together:
shared timer2.c and the selected wrapper .c

Reason:
the wrapper includes the shared implementation.
```

## Required files

```text
drivers/timers/timer2/timer2.h
drivers/timers/timer2/timer2.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
timer2_set_callback() callback from the application
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

Timer2 is a hardware backend with optional ISR callback dispatch.

## ISR requirements

Call `timer2_irq_handler()` from the timer interrupt.

## Resource ownership

Timer2 is project-owned when selected.

## Integration recipe

1. Add the shared Timer2 source or the selected wrapper.
2. Configure prescaler, period, and postscaler.
3. Install a callback if needed.
4. Call the IRQ handler from the ISR.

## Minimal usable example

```c
timer2_init(16u);
timer2_start();
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

Timer2 conflicts with other Timer2-backed modules.

## Extension points

Keep scheduling logic in higher-level modules.

## Common mistakes

- Sharing Timer2 between unrelated modules.
- Compiling wrapper and shared source together.

## AI decision rule

Use this driver for reusable Timer2 backend control.
