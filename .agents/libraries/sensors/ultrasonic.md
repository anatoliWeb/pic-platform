# ultrasonic

## Purpose

Ultrasonic distance sensor driver.

## Location

```text
libraries/sensors/ultrasonic/ultrasonic.h
libraries/sensors/ultrasonic/ultrasonic.c
```

## Use when

- you need reusable ultrasonic distance reads.

## Do not use when

- a different range sensor is required.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `ultrasonic_status_t` | enum | status | status values | status | none |  |
| `ultrasonic_config_t` | struct | config | trigger/echo/pulse timing | state | none |  |
| `ultrasonic_data_t` | struct | data | distance/time fields | state | none |  |
| `ultrasonic_t` | struct | state | runtime state | state | none |  |
| `ultrasonic_init` | `uint8_t ultrasonic_init(...);` | initialize sensor | config | success flag | stores config | exact args in header |
| `ultrasonic_trigger` | `ultrasonic_status_t ultrasonic_trigger(...);` | trigger measurement | sensor | status | emits trigger pulse | exact args in header |
| `ultrasonic_read` | `ultrasonic_status_t ultrasonic_read(...);` | read measurement | sensor, data | status | waits/polls | exact args in header |
| `ultrasonic_get_distance_mm` | `uint32_t ultrasonic_get_distance_mm(...);` | distance mm | sensor | distance | none | exact args in header |
| `ultrasonic_get_distance_cm` | `uint32_t ultrasonic_get_distance_cm(...);` | distance cm | sensor | distance | none | exact args in header |
| `ultrasonic_is_valid` | `uint8_t ultrasonic_is_valid(...);` | validity | sensor | flag | none | exact args in header |

## Configuration structures

Public fields are the config and state structs described in the header.

## Error/status model

Read helpers return a status enum.

## Source inclusion strategy

### Shared implementation

```text
libraries/sensors/ultrasonic/ultrasonic.c
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
libraries/sensors/ultrasonic/ultrasonic.c

For C18 add:
libraries/sensors/ultrasonic/ultrasonic.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/sensors/ultrasonic/ultrasonic.h
libraries/sensors/ultrasonic/ultrasonic.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
core/delay.h
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

Trigger and read operations are synchronous.

## ISR requirements

Not applicable for this module.

## Resource ownership

Trigger and echo pins are project-owned.

## Integration recipe

1. Add the shared ultrasonic source.
2. Fill the config.
3. Trigger and read distances.

## Minimal usable example

```c
ultrasonic_t sensor;
ultrasonic_init(&sensor, &cfg);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Measurement timing is hardware-specific.

## Common mistakes

- Ignoring trigger timing.
- Treating it as interrupt-driven if the API is polled.

## AI decision rule

Use this module for ultrasonic distance reads.
