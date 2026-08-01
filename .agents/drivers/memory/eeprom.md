# eeprom

## Purpose

Internal EEPROM driver.

## Location

```text
drivers/memory/eeprom/eeprom.h
drivers/memory/eeprom/eeprom.c
XC8/drivers/memory/eeprom/eeprom.c
C18/drivers/memory/eeprom/eeprom.c
```

## Use when

- you need reusable read/write access to internal EEPROM.

## Do not use when

- the project stores data in external I2C EEPROM instead.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `eeprom_init` | `void eeprom_init(void);` | initialize driver | none | none | configures EEPROM access |  |
| `eeprom_read_byte` | `uint8_t eeprom_read_byte(uint16_t address);` | read one byte | address | byte | EEPROM read |  |
| `eeprom_write_byte` | `void eeprom_write_byte(uint16_t address, uint8_t value);` | write one byte | address, value | none | EEPROM write |  |
| `eeprom_update_byte` | `void eeprom_update_byte(uint16_t address, uint8_t value);` | write if changed | address, value | none | EEPROM write |  |
| `eeprom_read_block` | `void eeprom_read_block(uint16_t address, uint8_t* buffer, uint16_t length);` | read block | address, buffer, length | none | EEPROM read |  |
| `eeprom_write_block` | `void eeprom_write_block(uint16_t address, const uint8_t* data, uint16_t length);` | write block | address, data, length | none | EEPROM write |  |

## Configuration structures

Not applicable for this module.

## Error/status model

Read/write helpers are void or return a byte value; error signaling is not explicit in the public API.

## Source inclusion strategy

### Shared implementation

```text
drivers/memory/eeprom/eeprom.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/memory/eeprom/eeprom.c
```

### C18 implementation/wrapper

```text
C18/drivers/memory/eeprom/eeprom.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/memory/eeprom/eeprom.c

For C18 add:
C18/drivers/memory/eeprom/eeprom.c

Do not add together:
shared eeprom.c and the selected wrapper .c

Reason:
the wrapper includes the shared implementation and duplicate symbols can result.
```

## Required files

```text
drivers/memory/eeprom/eeprom.h
drivers/memory/eeprom/eeprom.c
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

Operations are synchronous and blocking.

## ISR requirements

Not applicable for this module.

## Resource ownership

Internal EEPROM is owned by the MCU.

## Integration recipe

1. Add the shared EEPROM source or the selected wrapper.
2. Call read/write helpers by address.
3. Use update helpers to avoid redundant writes.

## Minimal usable example

```c
uint8_t value;

eeprom_init();
value = eeprom_read_byte(0u);
eeprom_write_byte(0u, value);
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

Address range is MCU-specific.

## Extension points

Keep higher-level storage formats in application code or reusable wrappers.

## Common mistakes

- Compiling wrapper and shared source together.
- Writing without checking address range in the caller.

## AI decision rule

Use this driver for internal EEPROM access.
