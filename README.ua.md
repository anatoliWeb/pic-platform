# PIC Drivers Platform (C18 + XC8) — Українська Версія

> English version: [README.md](README.md)

## Опис
PIC Drivers Platform — це бібліотека повторно використовуваних embedded-драйверів для PIC мікроконтролерів (основний фокус: PIC18).

- Цільова MCU сім'я: PIC18
- Підтримувані компілятори: MPLAB C18, MPLAB XC8
- Репозиторій містить платформу драйверів, а не готовий application

## Можливості
- GPIO
- UART / RS485
- ADC
- SPI / I2C
- 1-Wire / DS18B20
- LCD (GPIO + I2C)
- Button / Encoder
- Scheduler
- CRC / Ring Buffer

## Структура Проєкту
```text
/core
/drivers
/C18
/XC8
/examples
/docs
```

## Швидкий Старт
1. Скопіюйте потрібні папки драйверів з `/drivers`.
2. Скопіюйте потрібні спільні файли з `/core`.
3. Додайте реалізації C18 або XC8 з `/C18` або `/XC8`.
4. Підключіть заголовки та викличте `init()`.

## Приклад
```c
#include "core/compiler.h"
#include "drivers/uart/uart.h"

void main(void)
{
    uart_init(9600);
    uart_write_string("Hello");
}
```

## Документація
- Конвенції драйверів -> [docs/driver-convention.md](docs/driver-convention.md)
- Нотатки по компіляторах -> [docs/compiler.md](docs/compiler.md)
- Правила іменування -> [docs/naming.md](docs/naming.md)
- Робота з прикладами -> [docs/examples.md](docs/examples.md)
- Перенесення в новий проєкт -> [docs/porting.md](docs/porting.md)
- Валідація збірки -> [docs/build-validation.md](docs/build-validation.md)

## Принципи Дизайну
- без `malloc`
- неблокуючий підхід де це можливо
- повторне використання драйверів
- переносимість

## Статус
- активна розробка
- більшість драйверів стабільні

## Плани
- builder system
- config headers
