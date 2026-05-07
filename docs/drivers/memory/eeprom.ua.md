[🇬🇧 English version](./eeprom.md)

# EEPROM Драйвер

## Опис
Драйвер внутрішньої EEPROM для байтових і блокових операцій з енергонезалежним зберіганням.

## API
- `eeprom_init()`
- `eeprom_read_byte()`
- `eeprom_write_byte()`
- `eeprom_update_byte()`
- `eeprom_read_block()`
- `eeprom_write_block()`

## Приклад
```c
eeprom_write_byte(0x10u, 0x55u);
value = eeprom_read_byte(0x10u);
```

## Особливості
- Підтримує побайтові та блокові операції.
- `eeprom_update_byte()` зменшує зайві цикли запису.

## Залежності
- `core/compiler.h`
- `core/types.h`
