[🇬🇧 English version](./i2c.md)

# I2C Драйвер

## Опис

I2C драйвер на базі модуля MSSP для роботи у master mode.
Використовується для сканування пристроїв і обміну по регістрах із сенсорами та периферією.

## API

- `i2c_init()`
- `i2c_start()`
- `i2c_stop()`
- `i2c_write_byte()`
- `i2c_read_byte()`
- `i2c_scan()`

## Приклад

```c
#include "drivers/communication/i2c/i2c.h"

void main(void)
{
    uint8_t found[16];
    uint8_t reg_val = 0u;

    i2c_init(100000u);
    (void)i2c_scan(found, 16u);
    (void)i2c_read_register(0x48u, 0x00u, &reg_val);
}
```

## Особливості

- Обробка ACK/NACK входить у базовий read/write потік.
- Поточна реалізація орієнтована на master mode.
- Швидкість шини і таймінги залежать від налаштувань тактування.

## Залежності

- `core/compiler.h`
- `core/types.h`
- `core/device.h`
- `drivers/clock` (джерело частоти для таймінгів)

