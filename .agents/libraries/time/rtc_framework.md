# rtc_framework

## Purpose

Generic RTC framework.

## Location

```text
libraries/time/rtc_framework/rtc_framework.h
libraries/time/rtc_framework/rtc_framework.c
```

## Use when

- you need reusable calendar/time wrappers over RTC backends.

## Do not use when

- the project only needs a specific RTC chip driver.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `rtc_datetime_t` | struct | date/time | fields for date/time | state | none |  |
| `rtc_driver_t` | struct | driver vtable | backend callbacks | state | none |  |
| `rtc_t` | struct | RTC state | runtime state | state | none |  |
| `rtc_init` | `uint8_t rtc_init(...);` | initialize RTC | backend, state | success flag | stores backend | exact args in header |
| `rtc_set_datetime` | `uint8_t rtc_set_datetime(...);` | set datetime | rtc, datetime | status | backend write | exact args in header |
| `rtc_get_datetime` | `uint8_t rtc_get_datetime(...);` | get datetime | rtc, datetime | status | backend read | exact args in header |
| `rtc_set_unix` | `uint8_t rtc_set_unix(...);` | set unix time | rtc, unix | status | backend write | exact args in header |
| `rtc_get_unix` | `uint8_t rtc_get_unix(...);` | get unix time | rtc | unix | backend read | exact args in header |
| `rtc_tick` | `void rtc_tick(...);` | advance time | rtc | none | increments software state | exact args in header |
| `rtc_is_valid` | `uint8_t rtc_is_valid(...);` | validity | rtc | flag | none | exact args in header |

## Configuration structures

Public fields are the backend and state structs described in the header.

## Error/status model

Helpers return status flags.

## Source inclusion strategy

### Shared implementation

```text
libraries/time/rtc_framework/rtc_framework.c
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
libraries/time/rtc_framework/rtc_framework.c

For C18 add:
libraries/time/rtc_framework/rtc_framework.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/time/rtc_framework/rtc_framework.h
libraries/time/rtc_framework/rtc_framework.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
rtc_driver_t backend callbacks
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

The framework delegates storage and transport to the backend driver.

## ISR requirements

Not applicable for this module.

## Resource ownership

RTC backend ownership remains with the caller.

## Integration recipe

1. Add the shared RTC framework source.
2. Bind a backend driver.
3. Initialize and use datetime helpers.

## Minimal usable example

```c
rtc_t rtc;
rtc_init(&rtc, &driver, &backend);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Backend-specific behavior depends on the selected RTC chip.

## Common mistakes

- Mixing generic framework calls with a missing backend.

## AI decision rule

Use this module for reusable RTC abstraction.
