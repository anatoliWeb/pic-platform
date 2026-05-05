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

## Reset / Power Helper

Reset helper визначає причину останнього ресету через status-біти MCU.

Підтримувані причини:

- Power-on reset (POR)
- Brown-out reset (BOR)
- Watchdog reset (TO/WDT)
- External/software reset (RI)

API:

- `reset_init()`
- `reset_get_cause()`
- `reset_clear_flags()`

### Як це допомагає в debug

- можна вивести причину старту системи в UART log
- легше знайти випадкові WDT/BOR ресети

### Важливі нотатки

- логіка базується на RCON-бітах
- точна інтерпретація окремих бітів може відрізнятись між PIC18 моделями
- після аналізу причини рекомендується викликати `reset_clear_flags()`

### Приклади

- `drivers/reset/example.c`
- `C18/examples/reset_example.c`
- `XC8/examples/reset_example.c`
