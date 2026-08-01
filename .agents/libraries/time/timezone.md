# timezone

## Purpose

Timezone conversion helper.

## Location

```text
libraries/time/timezone/timezone.h
libraries/time/timezone/timezone.c
```

## Use when

- you need reusable UTC/local conversion helpers.

## Do not use when

- the project does not need timezone conversion.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `timezone_t` | struct | timezone state | offset/DST | state | none |  |
| `timezone_init` | `uint8_t timezone_init(...);` | initialize timezone | state, offset | success flag | stores config | exact args in header |
| `timezone_set_offset_minutes` | `void timezone_set_offset_minutes(...);` | set offset | timezone, offset | none | updates offset | exact args in header |
| `timezone_set_dst` | `void timezone_set_dst(...);` | set DST | timezone, flag | none | updates DST flag | exact args in header |
| `timezone_get_offset_minutes` | `int16_t timezone_get_offset_minutes(...);` | read offset | timezone | offset | none | exact args in header |
| `timezone_is_dst_enabled` | `uint8_t timezone_is_dst_enabled(...);` | read DST | timezone | flag | none | exact args in header |
| `timezone_utc_to_local` | `uint8_t timezone_utc_to_local(...);` | convert UTC | timezone, datetime | status | updates datetime | exact args in header |
| `timezone_local_to_utc` | `uint8_t timezone_local_to_utc(...);` | convert local | timezone, datetime | status | updates datetime | exact args in header |

## Configuration structures

Public fields are the `timezone_t` members described in the header.

## Error/status model

Helpers return a success flag.

## Source inclusion strategy

### Shared implementation

```text
libraries/time/timezone/timezone.c
```

### XC8 implementation/wrapper

```text
Not applicable for this module.
```

### C18 implementation/wrapper

```text
Not applicable for this module.
```

### Select exactly one route

```text
For XC8 add:
libraries/time/timezone/timezone.c

For C18 add:
libraries/time/timezone/timezone.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/time/timezone/timezone.h
libraries/time/timezone/timezone.c
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

Conversions are pure and synchronous.

## ISR requirements

Not applicable for this module.

## Resource ownership

Timezone state is caller-owned.

## Integration recipe

1. Add the shared timezone source.
2. Initialize offset/DST state.
3. Convert datetimes as needed.

## Minimal usable example

```c
timezone_t tz;
timezone_init(&tz, 120);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

DST rules are caller-managed.

## Common mistakes

- Treating it as a full calendar library.

## AI decision rule

Use this module for timezone conversions.
