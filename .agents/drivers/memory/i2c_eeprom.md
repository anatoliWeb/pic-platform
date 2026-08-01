# i2c_eeprom

## Purpose

External I2C EEPROM driver.

## Location

```text
drivers/memory/i2c_eeprom/i2c_eeprom.h
drivers/memory/i2c_eeprom/i2c_eeprom.c
XC8/drivers/memory/i2c_eeprom/i2c_eeprom.c
C18/drivers/memory/i2c_eeprom/i2c_eeprom.c
```

## Use when

- you need reusable access to external I2C EEPROM devices.

## Do not use when

- the project uses internal EEPROM instead.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `I2C_EEPROM_ADDR_WIDTH_8` | macro | address width | none | value | none |  |
| `I2C_EEPROM_ADDR_WIDTH_16` | macro | address width | none | value | none |  |
| `i2c_eeprom_config_t` | struct | driver config | bus address, page size, address width | struct | none |  |
| `i2c_eeprom_init` | `void i2c_eeprom_init(const i2c_eeprom_config_t* config);` | initialize driver | config ptr | none | stores config |  |
| `i2c_eeprom_is_ready` | `uint8_t i2c_eeprom_is_ready(void);` | probe ready state | none | ready flag | bus probe |  |
| `i2c_eeprom_write_byte` | `uint8_t i2c_eeprom_write_byte(uint16_t address, uint8_t data);` | write byte | address, data | status flag | I2C transaction |  |
| `i2c_eeprom_read_byte` | `uint8_t i2c_eeprom_read_byte(uint16_t address, uint8_t* data);` | read byte | address, data ptr | status flag | I2C transaction |  |
| `i2c_eeprom_write_buffer` | `uint8_t i2c_eeprom_write_buffer(uint16_t address, const uint8_t* data, uint16_t length);` | write buffer | address, data, length | status flag | I2C transaction |  |
| `i2c_eeprom_read_buffer` | `uint8_t i2c_eeprom_read_buffer(uint16_t address, uint8_t* data, uint16_t length);` | read buffer | address, data, length | status flag | I2C transaction |  |

## Configuration structures

| Field | Type | Required | Meaning | Valid range | Notes |
| --- | --- | --- | --- | --- | --- |
| `i2c_eeprom_config_t.address` | `uint8_t` | yes | device address | 7-bit I2C address |  |
| `i2c_eeprom_config_t.page_size` | `uint16_t` | yes | page size | device-specific |  |
| `i2c_eeprom_config_t.address_width` | enum | yes | address width | 8 or 16 bit |  |

## Error/status model

| Status/error | Meaning | Cause | Required handling |
| --- | --- | --- | --- |
| status flag | transaction success | bus or device error | retry or abort |

## Source inclusion strategy

### Shared implementation

```text
drivers/memory/i2c_eeprom/i2c_eeprom.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/memory/i2c_eeprom/i2c_eeprom.c
```

### C18 implementation/wrapper

```text
C18/drivers/memory/i2c_eeprom/i2c_eeprom.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/memory/i2c_eeprom/i2c_eeprom.c

For C18 add:
C18/drivers/memory/i2c_eeprom/i2c_eeprom.c

Do not add together:
shared i2c_eeprom.c and the selected wrapper .c

Reason:
the wrapper includes the shared implementation and duplicate symbols can result.
```

## Required files

```text
drivers/memory/i2c_eeprom/i2c_eeprom.h
drivers/memory/i2c_eeprom/i2c_eeprom.c
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

The module uses blocking I2C transactions and page-aware writes.

## ISR requirements

Not applicable for this module.

## Resource ownership

I2C bus and EEPROM address space are project-owned.

## Integration recipe

1. Add the shared I2C EEPROM source or the selected wrapper.
2. Initialize with the device config.
3. Read or write bytes and buffers.

## Minimal usable example

```c
i2c_eeprom_config_t cfg;
i2c_eeprom_init(&cfg);
(void)i2c_eeprom_is_ready();
```

## Working examples

Not applicable for this module.

## Proteus integration

Not applicable for this module.

## HEX artifacts

Not applicable for this module.

## Human documentation

Not applicable for this module.

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Page size and address width are device-specific.

## Extension points

Keep protocol-specific helpers in the reusable driver layer.

## Common mistakes

- Compiling wrapper and shared source together.
- Ignoring page boundaries during writes.

## AI decision rule

Use this driver for external I2C EEPROM access.
