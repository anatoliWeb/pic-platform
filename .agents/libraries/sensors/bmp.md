# bmp

## Purpose

BMP sensor driver.

## Location

```text
libraries/sensors/bmp/bmp.h
libraries/sensors/bmp/bmp.c
```

## Use when

- you need reusable BMP pressure/temperature reads.

## Do not use when

- a different sensor family is required.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `bmp_sensor_type_t` | enum | sensor type | `BMP_SENSOR_BMP180`, `BMP_SENSOR_BMP280` | type | none |  |
| `bmp_status_t` | enum | status | status values | status | none |  |
| `bmp_config_t` | struct | config | bus/address/calibration | state | none |  |
| `bmp_data_t` | struct | data | temperature/pressure/altitude | state | none |  |
| `bmp_t` | struct | state | runtime state | state | none |  |
| `bmp_init` | `uint8_t bmp_init(...);` | initialize sensor | config | success flag | stores config | exact args in header |
| `bmp_read` | `bmp_status_t bmp_read(...);` | read sensor | sensor, data | status | I2C transaction | exact args in header |
| `bmp_get_temperature` | `int32_t bmp_get_temperature(...);` | temperature | sensor | temperature | none | exact args in header |
| `bmp_get_pressure` | `uint32_t bmp_get_pressure(...);` | pressure | sensor | pressure | none | exact args in header |
| `bmp_get_altitude` | `int32_t bmp_get_altitude(...);` | altitude | sensor | altitude | none | exact args in header |
| `bmp_is_valid` | `uint8_t bmp_is_valid(...);` | validity | sensor | flag | none | exact args in header |

## Configuration structures

Public fields are the config and state structs described in the header.

## Error/status model

Read helper returns a status enum.

## Source inclusion strategy

### Shared implementation

```text
libraries/sensors/bmp/bmp.c
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
libraries/sensors/bmp/bmp.c

For C18 add:
libraries/sensors/bmp/bmp.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/sensors/bmp/bmp.h
libraries/sensors/bmp/bmp.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
drivers/communication/i2c/i2c.h
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

Reads use blocking I2C transactions.

## ISR requirements

Not applicable for this module.

## Resource ownership

I2C bus and sensor address are project-owned.

## Integration recipe

1. Add the shared BMP source.
2. Fill the config.
3. Initialize and read data.

## Minimal usable example

```c
bmp_t sensor;
bmp_init(&sensor, &cfg);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Specific calibration and protocol handling depends on sensor type.

## Common mistakes

- Using the wrong sensor type.
- Ignoring bus readiness.

## AI decision rule

Use this module for BMP sensor reads.
