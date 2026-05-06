[🇬🇧 English version](./spi.md)

# SPI Драйвер

## Опис

Драйвер SPI (MSSP) для платформи PIC.
Підтримує ініціалізацію master і базового slave режиму, побайтову та буферну передачу.

## API

- `spi_init_master()`
- `spi_init_slave()`
- `spi_transfer_byte()`
- `spi_transfer_buffer()`
- `spi_cs_select()`
- `spi_cs_deselect()`

## Приклад

```c
spi_init_master(SPI_MODE_0, SPI_CLOCK_FOSC_16);
spi_cs_init(&TRISB, &PORTB, 2u);

spi_cs_select();
spi_transfer_byte(0x8Fu);          // команда/регістр читання
value = spi_transfer_byte(0xFFu);  // прийом байта
spi_cs_deselect();
```

## Особливості

- Підтримуються режими SPI 0..3.
- Частота/подільник задається через `spi_clock_t`.
- Chip Select керується helper-функціями (`spi_cs_*`).

## Залежності

- `core/compiler.h`
- `core/types.h`

