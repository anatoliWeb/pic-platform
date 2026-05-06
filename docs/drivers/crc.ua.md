[🇬🇧 English version](./crc.md)

# CRC Драйвер

## Опис

Універсальний модуль перевірки цілісності даних для комунікаційних і storage-драйверів.
Надає обчислення CRC8, CRC16 і checksum.

## API

- `crc8_dallas()`
- `crc16_modbus()`
- `checksum8()`

## Приклад

```c
uint8_t frame[] = {0xAAu, 0x03u, 0x10u, 0x20u};
uint16_t crc = crc16_modbus(frame, 4u);

if (crc16_modbus(frame, 4u) == crc)
{
    // кадр валідний
}
```

## Особливості

- Реалізація циклами, без lookup table.
- Легка та портативна для embedded-цілей.
- Використовується у протокольних шарах, зокрема RS485 і перевірках для 1-Wire.

## Залежності

- `core/compiler.h`
- `core/types.h`

