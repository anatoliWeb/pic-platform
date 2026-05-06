[🇬🇧 English version](./onewire.md)

# OneWire Драйвер

## Опис

Базовий 1-Wire драйвер (bit-banging) через GPIO.
Реалізує reset/presence шини, побітову/побайтову передачу та ROM-команди.

## API

- `onewire_init()`
- `onewire_reset()`
- `onewire_write_bit()`
- `onewire_read_bit()`
- `onewire_write_byte()`
- `onewire_read_byte()`
- `onewire_skip_rom()`
- `onewire_match_rom()`
- `onewire_read_rom()`
- `onewire_search_rom()`
- `onewire_crc8()`

## Приклад

```c
uint8_t rom[8];

onewire_init(&PORTB, &TRISB, 0u);
if (onewire_reset() != 0u)
{
    onewire_read_rom(rom);
}
```

## Особливості

- Протокол чутливий до таймінгів; використовує затримки у мікросекундах.
- ROM-команди покривають сценарії single-device і адресного доступу.

## Залежності

- `core/compiler.h`
- `core/types.h`
- `core/delay.h`
- `core/interrupts.h`

