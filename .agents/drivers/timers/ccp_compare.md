# ccp_compare

## Purpose

CCP compare mode driver.

## Location

```text
drivers/timers/ccp_compare/ccp_compare.h
drivers/timers/ccp_compare/ccp_compare.c
XC8/drivers/timers/ccp_compare/ccp_compare.c
C18/drivers/timers/ccp_compare/ccp_compare.c
```

## Use when

- you need reusable compare-match interrupt handling.

## Do not use when

- compare mode is not part of the project.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `ccp_compare_callback_t` | function pointer typedef | callback type | compare event | none | callback |  |
| `ccp_compare_init` | `void ccp_compare_init(void);` | initialize module | none | none | configures hardware |  |
| `ccp_compare_enable` | `void ccp_compare_enable(void);` | enable compare | none | none | sets enable bit |  |
| `ccp_compare_disable` | `void ccp_compare_disable(void);` | disable compare | none | none | clears enable bit |  |
| `ccp_compare_set_value` | `void ccp_compare_set_value(uint16_t value);` | set compare value | value | none | updates compare register |  |
| `ccp_compare_set_callback` | `void ccp_compare_set_callback(ccp_compare_callback_t cb);` | install callback | callback | none | stores callback |  |
| `ccp_compare_irq_handler` | `void ccp_compare_irq_handler(void);` | ISR dispatcher | none | none | invokes callback |  |

## Configuration structures

Not applicable for this module.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
drivers/timers/ccp_compare/ccp_compare.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/timers/ccp_compare/ccp_compare.c
```

### C18 implementation/wrapper

```text
C18/drivers/timers/ccp_compare/ccp_compare.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/timers/ccp_compare/ccp_compare.c

For C18 add:
C18/drivers/timers/ccp_compare/ccp_compare.c

Do not add together:
shared ccp_compare.c and the selected wrapper .c

Reason:
the wrapper includes the shared implementation.
```

## Required files

```text
drivers/timers/ccp_compare/ccp_compare.h
drivers/timers/ccp_compare/ccp_compare.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
ccp_compare_callback_t callback from the application
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

Compare events are delivered via the ISR dispatcher.

## ISR requirements

Call `ccp_compare_irq_handler()` from the compare interrupt.

## Resource ownership

CCP compare peripheral ownership is project-specific.

## Integration recipe

1. Add the shared CCP compare source or the selected wrapper.
2. Set compare value and callback.
3. Enable the module.
4. Call the IRQ handler from the ISR.

## Minimal usable example

```c
ccp_compare_init();
ccp_compare_set_value(1000u);
ccp_compare_enable();
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

Compare channel ownership is hardware-specific.

## Extension points

Keep compare scheduling in higher-level reusable code.

## Common mistakes

- Forgetting to call the IRQ handler.
- Compiling wrapper and shared source together.

## AI decision rule

Use this driver for reusable compare-match handling.
