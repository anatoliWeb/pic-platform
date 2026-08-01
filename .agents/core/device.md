# device

## Purpose

Project clock glue and device-level frequency resolution.

## Location

```text
core/device.h
core/config/project_config_template.h
```

## Use when

- a module needs `_XTAL_FREQ`, `DRV_XTAL_FREQ`, or `PIC_PLATFORM_CLOCK_HZ`;
- a project needs a default clock fallback policy.

## Do not use when

- the clock is already fixed in project config and no abstraction is needed.

## Public API

| Function/type/macro | Exact signature or type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `PIC_PLATFORM_CLOCK_HZ` | macro | source of truth for clock | value | none | compile-time clock value | preferred |
| `DRV_XTAL_FREQ` | macro | driver clock alias | value | none | compile-time clock value | derived from `PIC_PLATFORM_CLOCK_HZ` |
| `_XTAL_FREQ` | macro | XC8 compatibility | value | none | compile-time clock value | derived from `PIC_PLATFORM_CLOCK_HZ` |

## Configuration structures

```text
project_config_template.h defines the template flags and clock policy placeholders
```

## Source inclusion strategy

### Shared implementation

```text
header-only
```

### XC8 implementation/wrapper

```text
none
```

### C18 implementation/wrapper

```text
none
```

### Select exactly one route

```text
include once in modules that need clock macros
```

## Dependencies

```text
core/config/project_config_template.h
```

## Runtime model

- Compile-time only.

## Resource ownership

```text
clock configuration only
```
