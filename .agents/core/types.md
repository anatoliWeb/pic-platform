# types

## Purpose

Shared platform enums and boolean/status types.

## Location

```text
core/types.h
```

## Use when

- you need `drv_status_t` or `drv_bool_t` in reusable code.

## Do not use when

- a plain C type already fits and no platform status type is needed.

## Public API

| Function/type | Purpose | What to pass | Returns / output | Notes |
| --- | --- | --- | --- | --- |
| `drv_status_t` | shared status enum | nothing | `DRV_STATUS_OK`, `DRV_STATUS_ERROR`, `DRV_STATUS_TIMEOUT`, `DRV_STATUS_UNSUPPORTED` | used widely in libraries |
| `drv_bool_t` | shared boolean enum | nothing | `DRV_FALSE`, `DRV_TRUE` | keep if module already uses it |

## Required files

```text
core/types.h
```

## Dependencies

```text
core/compiler.h
```

## Runtime model

- Header-only.

## Integration recipe

1. Include `core/types.h` in reusable code.
2. Return `drv_status_t` from library init and control functions.

## Human documentation

```text
docs/architecture/module-inventory.md
```

## AI decision rule

Use these shared types instead of inventing per-module status enums unless a module genuinely needs its own error model.
