# software_rtc

## Purpose

Software RTC helper.

## Location

```text
libraries/time/software_rtc/software_rtc.h
libraries/time/software_rtc/software_rtc.c
```

## Use when

- you need an RTC backed by software tick updates.

## Do not use when

- a hardware RTC chip is available and preferred.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `software_rtc_t` | struct | RTC state | runtime state | state | none |  |
| `software_rtc_init` | `uint8_t software_rtc_init(...);` | initialize RTC | state, start time | success flag | stores state | exact args in header |
| `software_rtc_tick_ms` | `void software_rtc_tick_ms(...);` | tick ms | rtc | none | advances time | exact args in header |
| `software_rtc_tick_seconds` | `void software_rtc_tick_seconds(...);` | tick seconds | rtc | none | advances time | exact args in header |
| `software_rtc_set_datetime` | `uint8_t software_rtc_set_datetime(...);` | set datetime | rtc, datetime | status | updates state | exact args in header |
| `software_rtc_get_datetime` | `uint8_t software_rtc_get_datetime(...);` | get datetime | rtc, datetime | status | reads state | exact args in header |
| `software_rtc_sync` | `void software_rtc_sync(...);` | sync state | rtc | none | updates derived fields | exact args in header |
| `software_rtc_is_running` | `uint8_t software_rtc_is_running(...);` | running state | rtc | flag | none | exact args in header |
| `software_rtc_get_driver` | `const rtc_driver_t* software_rtc_get_driver(void);` | get backend driver | none | driver ptr | none |  |

## Configuration structures

Public fields are the state struct described in the header.

## Error/status model

Init returns success flag.

## Source inclusion strategy

### Shared implementation

```text
libraries/time/software_rtc/software_rtc.c
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
libraries/time/software_rtc/software_rtc.c

For C18 add:
libraries/time/software_rtc/software_rtc.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/time/software_rtc/software_rtc.h
libraries/time/software_rtc/software_rtc.c
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

The RTC is advanced explicitly by tick calls.

## ISR requirements

Not applicable for this module.

## Resource ownership

State ownership stays with the caller.

## Integration recipe

1. Add the shared software RTC source.
2. Initialize state.
3. Feed millisecond or second ticks.

## Minimal usable example

```c
software_rtc_t rtc;
software_rtc_init(&rtc, &start);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Accuracy depends on the supplied tick source.

## Common mistakes

- Forgetting to feed ticks.
- Using it as a hardware timekeeper.

## AI decision rule

Use this module for software-backed RTC state.
