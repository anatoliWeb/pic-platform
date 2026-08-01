# i2c

## Purpose

Reusable I2C master driver.

## Location

```text
drivers/communication/i2c/i2c.h
drivers/communication/i2c/i2c.c
XC8/drivers/communication/i2c/i2c.c
C18/drivers/communication/i2c/i2c.c
```

## Use when

- you need reusable I2C transactions;
- you need register access helpers for sensor or EEPROM drivers.

## Do not use when

- the project uses another bus protocol.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `i2c_init` | `void i2c_init(uint32_t clock_hz);` | initialize bus | clock frequency | none | configures hardware |  |
| `i2c_start` | `void i2c_start(void);` | start condition | none | none | bus transaction |  |
| `i2c_restart` | `void i2c_restart(void);` | repeated start | none | none | bus transaction |  |
| `i2c_stop` | `void i2c_stop(void);` | stop condition | none | none | bus transaction |  |
| `i2c_write_byte` | `uint8_t i2c_write_byte(uint8_t data);` | write one byte | data byte | ack/nack flag | bus transaction |  |
| `i2c_read_byte` | `uint8_t i2c_read_byte(uint8_t ack);` | read one byte | ack flag | byte read | bus transaction |  |
| `i2c_write_register` | `uint8_t i2c_write_register(uint8_t device_addr, uint8_t reg_addr, uint8_t data);` | write register | device, reg, data | status flag | bus transaction |  |
| `i2c_read_register` | `uint8_t i2c_read_register(uint8_t device_addr, uint8_t reg_addr, uint8_t* data);` | read register | device, reg, output ptr | status flag | bus transaction |  |
| `i2c_device_ready` | `uint8_t i2c_device_ready(uint8_t device_addr);` | probe device | address | status flag | bus transaction |  |
| `i2c_scan` | `uint8_t i2c_scan(uint8_t* found_addresses, uint8_t max_devices);` | scan bus | output array, max count | count/status | bus transactions |  |

## Configuration structures

Not applicable for this module.

## Error/status model

| Status/error | Meaning | Cause | Required handling |
| --- | --- | --- | --- |
| ack/nack flag | byte accepted or not | slave response | branch on returned flag |
| scan count | number of devices found | bus probe result | use as discovery count |

## Source inclusion strategy

### Shared implementation

```text
drivers/communication/i2c/i2c.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/communication/i2c/i2c.c
```

### C18 implementation/wrapper

```text
C18/drivers/communication/i2c/i2c.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/communication/i2c/i2c.c

For C18 add:
C18/drivers/communication/i2c/i2c.c

Do not add together:
shared i2c.c and the selected wrapper .c

Reason:
the wrapper includes the shared implementation and compiling both duplicates symbols.
```

## Required files

```text
drivers/communication/i2c/i2c.h
drivers/communication/i2c/i2c.c
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

| Define | Default | Where | Effect | Required files | Resource impact | Code-size impact |
| --- | ---: | --- | --- | --- | --- | --- |
| none module-specific | n/a | n/a | bus speed is runtime-configured | shared and wrapper sources | I2C pins | small |

## Runtime model

Master bus operations are synchronous and blocking.

## ISR requirements

Not applicable for this module.

## Resource ownership

I2C pins are project-owned.

## Integration recipe

1. Add the shared I2C source or the selected compiler wrapper.
2. Initialize the bus frequency.
3. Use start/write/read/stop helpers for transactions.
4. Use register helpers for peripheral drivers.

## Minimal usable example

```c
i2c_init(100000UL);
i2c_start();
(void)i2c_write_byte(0xA0u);
i2c_stop();
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

Blocking bus access.

## Extension points

Keep higher-level device logic in drivers or libraries that call this bus layer.

## Common mistakes

- Adding both shared and wrapper source files.
- Using the bus without checking ack status.

## AI decision rule

Use this driver for reusable I2C transactions and register helpers.
