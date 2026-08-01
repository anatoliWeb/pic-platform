# dht

## Purpose

DHT sensor driver.

## Location

```text
libraries/sensors/dht/dht.h
libraries/sensors/dht/dht.c
```

## Use when

- you need reusable DHT temperature/humidity reads.

## Do not use when

- a different sensor family is required.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `dht_sensor_type_t` | enum | sensor type | `DHT_SENSOR_DHT11`, `DHT_SENSOR_DHT22` | type | none |  |
| `dht_status_t` | enum | status | status values | status | none |  |
| `dht_config_t` | struct | config | pin/timing/type | state | none |  |
| `dht_data_t` | struct | data | temperature/humidity | state | none |  |
| `dht_t` | struct | state | runtime state | state | none |  |
| `dht_init` | `uint8_t dht_init(...);` | initialize sensor | config | success flag | stores config | exact args in header |
| `dht_read` | `dht_status_t dht_read(...);` | read sensor | sensor, data | status | bit-bang transaction | exact args in header |
| `dht_get_temperature` | `int16_t dht_get_temperature(...);` | temperature | sensor | temperature | none | exact args in header |
| `dht_get_humidity` | `uint16_t dht_get_humidity(...);` | humidity | sensor | humidity | none | exact args in header |
| `dht_is_valid` | `uint8_t dht_is_valid(...);` | validity | sensor | flag | none | exact args in header |

## Configuration structures

Public fields are the config and state structs described in the header.

## Error/status model

Read helper returns a status enum.

## Source inclusion strategy

### Shared implementation

```text
libraries/sensors/dht/dht.c
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
libraries/sensors/dht/dht.c

For C18 add:
libraries/sensors/dht/dht.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/sensors/dht/dht.h
libraries/sensors/dht/dht.c
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

Reads are timing-sensitive and blocking.

## ISR requirements

Not applicable for this module.

## Resource ownership

DHT data pin is project-owned.

## Integration recipe

1. Add the shared DHT source.
2. Fill the config.
3. Initialize and read data.

## Minimal usable example

```c
dht_t sensor;
dht_init(&sensor, &cfg);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Timing-sensitive bit-bang protocol.

## Common mistakes

- Using the wrong sensor type.
- Ignoring delay timing.

## AI decision rule

Use this module for DHT sensor reads.
