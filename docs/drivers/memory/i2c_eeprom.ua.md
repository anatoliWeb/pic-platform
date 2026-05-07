[🇬🇧 English version](./i2c_eeprom.md)

# Драйвер I2C EEPROM

## Опис
Універсальний драйвер зовнішньої EEPROM серії 24Cxx через I2C.
Це рівень драйвера пам'яті, який залежить лише від core та драйвера I2C.

## Підтримувані мікросхеми
- 24C01
- 24C02
- 24C04
- 24C08
- 24C16
- 24C32
- 24C64
- 24C256

## Публічний API
- `i2c_eeprom_init()`
- `i2c_eeprom_is_ready()`
- `i2c_eeprom_write_byte()`
- `i2c_eeprom_read_byte()`
- `i2c_eeprom_write_buffer()`
- `i2c_eeprom_read_buffer()`

## Модель конфігурації
Використовується `i2c_eeprom_config_t`:
- `device_address`: базова 7-бітна адреса (зазвичай `0x50`)
- `page_size`: розмір сторінки для page write
- `address_width`: `I2C_EEPROM_ADDR_WIDTH_8` або `I2C_EEPROM_ADDR_WIDTH_16`
- `ready_retries`: кількість спроб polling після запису
- `write_cycle_ms`: пауза між перевірками готовності

## Нотатки по адресації
- Для 8-бітної адреси пам'яті (малі 24Cxx) використовуються block-біти в control address.
- Для 16-бітної адресації (24C32+) передаються старший і молодший байти адреси.

## Обмеження page write
`i2c_eeprom_write_buffer()` автоматично ділить запис по межах сторінок.

## Типове підключення
- VCC -> 3.3V/5V
- GND -> GND
- SDA -> SDA MCU
- SCL -> SCL MCU
- WP -> GND (дозвіл запису) або VCC (захист)
- A0/A1/A2 -> вибір I2C-адреси

## Приклад
```c
#include "drivers/communication/i2c/i2c.h"
#include "drivers/memory/i2c_eeprom/i2c_eeprom.h"

i2c_eeprom_config_t cfg = {0x50u, 16u, I2C_EEPROM_ADDR_WIDTH_8, 40u, 5u};
uint8_t value;

i2c_init(100000u);
i2c_eeprom_init(&cfg);

i2c_eeprom_write_byte(0x0000u, 0x5Au);
i2c_eeprom_read_byte(0x0000u, &value);
```

## Залежності
- `core/*`
- `drivers/communication/i2c/*`