# spi

## Purpose

Reusable SPI master/slave driver.

## Location

```text
drivers/communication/spi/spi.h
drivers/communication/spi/spi.c
XC8/drivers/communication/spi/spi.c
C18/drivers/communication/spi/spi.c
```

## Use when

- you need reusable SPI bus transfers and chip-select helpers.

## Do not use when

- the project does not need SPI.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `spi_mode_t` | enum | SPI mode | `SPI_MODE_0..3` | mode | none |  |
| `spi_clock_t` | enum | clock divider | `SPI_CLOCK_FOSC_4`, `_16`, `_64`, `_TMR2` | clock setting | none |  |
| `spi_init_master` | `void spi_init_master(spi_mode_t mode, spi_clock_t clock);` | init master | mode, clock | none | configures SPI hardware |  |
| `spi_init_slave` | `void spi_init_slave(spi_mode_t mode);` | init slave | mode | none | configures SPI hardware |  |
| `spi_transfer_byte` | `uint8_t spi_transfer_byte(uint8_t data);` | transfer one byte | data | byte | SPI transaction |  |
| `spi_transfer_buffer` | `void spi_transfer_buffer(const uint8_t* tx_buffer, uint8_t* rx_buffer, uint16_t length);` | transfer a buffer | tx/rx buffers, length | none | SPI transaction |  |
| `spi_cs_init` | `void spi_cs_init(volatile uint8_t* tris, volatile uint8_t* port, uint8_t pin);` | bind CS pin | TRIS, PORT, pin | none | configures chip-select |  |
| `spi_cs_select` | `void spi_cs_select(void);` | assert CS | none | none | changes CS pin |  |
| `spi_cs_deselect` | `void spi_cs_deselect(void);` | deassert CS | none | none | changes CS pin |  |

## Configuration structures

Not applicable for this module.

## Error/status model

Transfer helpers return the byte read back from the bus or void for buffered transfer.

## Source inclusion strategy

### Shared implementation

```text
drivers/communication/spi/spi.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/communication/spi/spi.c
```

### C18 implementation/wrapper

```text
C18/drivers/communication/spi/spi.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/communication/spi/spi.c

For C18 add:
C18/drivers/communication/spi/spi.c

Do not add together:
shared spi.c and the selected wrapper .c

Reason:
the wrapper compiles the shared source and duplicate symbols are possible.
```

## Required files

```text
drivers/communication/spi/spi.h
drivers/communication/spi/spi.c
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

SPI mode and clock are configured once, then transfers are synchronous.

## ISR requirements

Not applicable for this module.

## Resource ownership

SPI pins and chip-select pins are project-owned.

## Integration recipe

1. Add the shared SPI source or the selected compiler wrapper.
2. Initialize master or slave mode.
3. Bind chip-select when needed.
4. Transfer bytes or buffers.

## Minimal usable example

```c
spi_init_master(SPI_MODE_0, SPI_CLOCK_FOSC_4);
spi_cs_select();
(void)spi_transfer_byte(0xAAu);
spi_cs_deselect();
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

Hardware pin mapping is project-specific.

## Extension points

Keep protocol logic in higher-level reusable drivers.

## Common mistakes

- Compiling wrapper and shared source together.
- Forgetting to bind chip-select.

## AI decision rule

Use this driver for reusable SPI bus access.
