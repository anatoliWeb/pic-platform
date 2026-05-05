# PIC PLATFORM - Universal Drivers (C18 + XC8)

Бібліотека універсальних драйверів для PIC (фокус: PIC18).
Це база драйверів, не application-проєкт.

## Підтримувані компілятори

- MPLAB C18
- MPLAB XC8

## Архітектура

- Спільний API для кожного драйвера
- Universal entrypoint у `drivers/`
- Compiler-specific реалізації у `C18/` і `XC8/`
- Fallback реалізація, якщо override не вибрано

## Comparator Driver

Comparator driver призначений для простого threshold detection.

Підтримка:

- `comparator_init(mode)`
- `comparator_enable()`
- `comparator_disable()`
- `comparator_get_output()`

Режими:

- `CMP_MODE_OFF`
- `CMP_MODE_1`
- `CMP_MODE_2`
- `CMP_MODE_3`

Налаштування режиму виконується через `CMCON` / `CMCON0` залежно від MCU.

### Comparator Driver Architecture

- `drivers/comparator/comparator.c` — universal entrypoint + fallback
- `C18/drivers/comparator/comparator.c` — C18-specific implementation
- `XC8/drivers/comparator/comparator.c` — XC8-specific implementation

### Threshold Detection Example

- init comparator
- read comparator output
- якщо output=1 -> toggle/set LED
- якщо output=0 -> clear LED

Приклади:

- `drivers/comparator/example.c`
- `C18/examples/comparator_example.c`
- `XC8/examples/comparator_example.c`

## Інші драйвери

Доступні: GPIO, UART, UART debug, RS485, ADC, PWM, Timer, EEPROM, SPI, I2C, external interrupt, PORTB change, WDT.
