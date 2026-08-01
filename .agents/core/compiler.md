# compiler

## Purpose

Compiler abstraction for XC8 and C18.

## Location

```text
core/compiler.h
```

## Use when

- you need portable compiler macros;
- you need interrupt enable/disable helpers;
- you need XC8/C18 type compatibility.

## Do not use when

- you are writing project-specific business logic;
- you can solve it inside a driver or library header without compiler branching.

## Public API

| Function/type | Purpose | What to pass | Returns / output | Notes |
| --- | --- | --- | --- | --- |
| `DRV_COMPILER_XC8`, `DRV_COMPILER_C18` | compiler selection macros | nothing | compile-time flags | do not define both |
| `DRV_INT_DISABLE()` / `DRV_INT_ENABLE()` | global interrupt control | nothing | macro side effect | uses `INTCONbits.GIE` |
| `DRV_INLINE`, `DRV_ROM`, `DRV_UNUSED` | compiler compatibility helpers | values/macros | macro output | keep in core |

## Required files

### Core/library files

```text
core/compiler.h
```

### XC8 wrapper files

```text
none
```

### C18 wrapper files

```text
none
```

## Dependencies

```text
compiler built-ins
```

## Configuration defines

| Define | Default | Where | Effect | Code size impact |
| --- | ---: | --- | --- | --- |
| `DRV_COMPILER_XC8` | n/a | project/compiler selection | selects XC8 branch | none |
| `DRV_COMPILER_C18` | n/a | project/compiler selection | selects C18 branch | none |

## Runtime model

- Header-only abstraction.

## Integration recipe

1. Include `core/compiler.h`.
2. Use `DRV_*` macros instead of compiler-specific branching.
3. Keep device headers outside reusable code.

## Minimal usage example

```c
#include "core/compiler.h"

DRV_INT_DISABLE();
DRV_INT_ENABLE();
```

## Resource ownership

- No peripheral ownership.

## Examples

```text
All platform modules
```

## Human documentation

```text
docs/compiler.md
docs/compiler.ua.md
```

## Known limitations

- Only XC8 and C18 are documented in the repo.

## Extension points

- Extend only if a new compiler backend is реально added to the repo.

## AI decision rule

When a task needs compiler branching, keep it here or in a wrapper, not in project-local logic.
