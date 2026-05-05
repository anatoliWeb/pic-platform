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

## Watchdog Timer (WDT)

WDT helper є мінімальним модулем для безпечного очищення watchdog.

Що включено:

- `wdt_clear()`
- `WDT_CLEAR()` macro alias

WDT зазвичай вмикається через config bits, а не runtime API.
На багатьох PIC18 вмикання/вимикання WDT у runtime обмежене або недоступне.

### Як використовувати

- Викликайте `wdt_clear()` у головному циклі після критичних ділянок логіки
- Не викликайте `wdt_clear()` занадто часто без контролю, щоб не приховати зависання

### Config Notes

- Увімкнення/режим WDT задається у configuration bits проєкту
- Prescaler WDT також задається config-параметрами конкретного MCU

### Приклади

- `drivers/wdt/example.c`
- `C18/examples/wdt_example.c`
- `XC8/examples/wdt_example.c`

## Інші драйвери

Доступні: GPIO, UART, UART debug, RS485, ADC, PWM, Timer, EEPROM, SPI, I2C, external interrupt, portb change.
