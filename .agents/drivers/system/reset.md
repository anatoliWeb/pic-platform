# reset

## Purpose

Reset cause driver.

## Location

```text
drivers/system/reset/reset.h
drivers/system/reset/reset.c
XC8/drivers/system/reset/reset.c
C18/drivers/system/reset/reset.c
```

## Use when

- you need reusable reset-cause inspection.

## Do not use when

- the project never inspects reset causes.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `reset_cause_t` | enum | reset cause | `RESET_CAUSE_UNKNOWN`, `POWER_ON`, `BROWN_OUT`, `WATCHDOG`, `EXTERNAL`, `SOFTWARE` | cause | none |  |
| `reset_init` | `void reset_init(void);` | initialize module | none | none | captures flags |  |
| `reset_get_cause` | `reset_cause_t reset_get_cause(void);` | get cause | none | cause enum | none |  |
| `reset_clear_flags` | `void reset_clear_flags(void);` | clear reset flags | none | none | clears hardware flags |  |

## Configuration structures

Not applicable for this module.

## Error/status model

The cause enum is the status model.

## Source inclusion strategy

### Shared implementation

```text
drivers/system/reset/reset.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/system/reset/reset.c
```

### C18 implementation/wrapper

```text
C18/drivers/system/reset/reset.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/system/reset/reset.c

For C18 add:
C18/drivers/system/reset/reset.c

Do not add together:
shared reset.c and the selected wrapper .c

Reason:
the wrapper includes the shared implementation.
```

## Required files

```text
drivers/system/reset/reset.h
drivers/system/reset/reset.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

Not applicable for this module.

### Optional feature dependencies

Not applicable for this module.

### Example-only dependencies

Not applicable for this module.

### Transitive dependencies

Not applicable for this module.

## Compile-time defines

Not applicable for this module.

## Runtime model

Reset cause is sampled after startup and cleared on request.

## ISR requirements

Not applicable for this module.

## Resource ownership

Reset source flags belong to the MCU.

## Integration recipe

1. Add the shared reset source or the selected wrapper.
2. Call `reset_init()` early.
3. Inspect and clear the cause as needed.

## Minimal usable example

```c
reset_init();
switch (reset_get_cause())
{
}
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

Reset causes depend on device flags.

## Extension points

Add higher-level diagnostics in reusable code rather than duplicating reset logic.

## Common mistakes

- Forgetting to clear flags.
- Compiling wrapper and shared source together.

## AI decision rule

Use this driver for reset-cause inspection.
