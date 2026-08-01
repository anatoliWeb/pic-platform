# wdt

## Purpose

Watchdog clear helper.

## Location

```text
drivers/system/wdt/wdt.h
drivers/system/wdt/wdt.c
XC8/drivers/system/wdt/wdt.c
C18/drivers/system/wdt/wdt.c
```

## Use when

- you need a reusable watchdog clear helper.

## Do not use when

- the project does not use the watchdog.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `wdt_clear` | `void wdt_clear(void);` | clear watchdog | none | none | resets watchdog timer |  |
| `WDT_CLEAR()` | macro | clear watchdog | none | none | macro expansion | convenience |

## Configuration structures

Not applicable for this module.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
drivers/system/wdt/wdt.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/system/wdt/wdt.c
```

### C18 implementation/wrapper

```text
C18/drivers/system/wdt/wdt.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/system/wdt/wdt.c

For C18 add:
C18/drivers/system/wdt/wdt.c

Do not add together:
shared wdt.c and the selected wrapper .c

Reason:
the wrapper includes the shared implementation.
```

## Required files

```text
drivers/system/wdt/wdt.h
drivers/system/wdt/wdt.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
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

Clearing the watchdog is a direct hardware action.

## ISR requirements

Not applicable for this module.

## Resource ownership

Watchdog timing belongs to the MCU and project configuration.

## Integration recipe

1. Add the shared watchdog source or the selected wrapper.
2. Call `wdt_clear()` periodically if watchdog is enabled.

## Minimal usable example

```c
WDT_CLEAR();
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

Watchdog behavior is device-specific.

## Extension points

Keep watchdog policy in project code.

## Common mistakes

- Compiling wrapper and shared source together.
- Forgetting to clear the watchdog in long-running loops.

## AI decision rule

Use this driver for reusable watchdog clearing.
