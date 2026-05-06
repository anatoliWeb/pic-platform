[🇺🇦 Ukrainian version](./spi.ua.md)

# SPI Driver

## Description

SPI (MSSP) communication driver for PIC platform.
Supports master and basic slave initialization, single-byte and buffer transfers.

## API

- `spi_init_master()`
- `spi_init_slave()`
- `spi_transfer_byte()`
- `spi_transfer_buffer()`
- `spi_cs_select()`
- `spi_cs_deselect()`

## Example

```c
spi_init_master(SPI_MODE_0, SPI_CLOCK_FOSC_16);
spi_cs_init(&TRISB, &PORTB, 2u);

spi_cs_select();
spi_transfer_byte(0x8Fu);          // read command/register
value = spi_transfer_byte(0xFFu);  // receive data
spi_cs_deselect();
```

## Notes

- Supports SPI modes 0..3.
- Clock source/divider is configured via `spi_clock_t`.
- Chip Select is handled by helper functions (`spi_cs_*`).

## Dependencies

- `core/compiler.h`
- `core/types.h`

