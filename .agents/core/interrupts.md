# interrupts

## Purpose

Interrupt control facade.

## Location

```text
core/interrupts.h
core/compiler.h
```

## Public API

| Function/type/macro | Exact signature or type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `DRV_INT_ENABLE` | macro | enable global interrupts | none | none | sets GIE | defined in compiler.h |
| `DRV_INT_DISABLE` | macro | disable global interrupts | none | none | clears GIE | defined in compiler.h |

## Source inclusion strategy

```text
header-only
```
