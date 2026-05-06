# PIC Drivers Platform (C18 + XC8)

> [English version](README.md)

## Опис
PIC Drivers Platform — це бібліотека повторно використовуваних embedded-драйверів для PIC мікроконтролерів (фокус на PIC18).
Вона надає портативну базу для низькорівневих драйверів і інтерфейсів.
Це драйверна платформа, а не application.

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
1. Скопіювати потрібні драйвери з `drivers/`.
2. Додати `core/` та include paths у проєкт.
3. Підключити C18/XC8-реалізації за потреби.
4. Підключити заголовки і викликати `init()`.

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

## Підтримувані Компілятори
- MPLAB C18
- MPLAB XC8

## Документація
- Конвенція драйверів: [docs/driver-convention.md](docs/driver-convention.md) | [UA](docs/driver-convention.ua.md)
- Нотатки про компілятори: [docs/compiler.md](docs/compiler.md) | [UA](docs/compiler.ua.md)
- Правила іменування: [docs/naming.md](docs/naming.md) | [UA](docs/naming.ua.md)
- Гайд прикладів: [docs/examples.md](docs/examples.md) | [UA](docs/examples.ua.md)
- Гайд портування: [docs/porting.md](docs/porting.md) | [UA](docs/porting.ua.md)
- Валідація збірки: [docs/build-validation.md](docs/build-validation.md) | [UA](docs/build-validation.ua.md)
