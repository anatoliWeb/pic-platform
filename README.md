# PIC PLATFORM - Universal Drivers (C18 + XC8)

Бібліотека універсальних драйверів для PIC (фокус: PIC18).

## Підтримувані компілятори

- MPLAB C18
- MPLAB XC8

## Архітектура

- Спільний API для кожного драйвера
- Universal entrypoint у `drivers/`
- Compiler-specific реалізації у `C18/` і `XC8/`
- Fallback реалізація, якщо override не вибрано

## CCP Capture / Compare

CCP Capture та CCP Compare реалізовані як два окремі драйвери.
Вони використовують Timer1 як базу часу і callback-підхід для подій.

### CCP Capture

- capture по rising/falling або кожні 4/16 подій
- зчитування 16-bit значення з CCPR
- callback `ccp_capture_callback_t(value)`
- `ccp_capture_irq_handler()` викликається з ISR проєкту

### CCP Compare

- встановлення compare value через CCPR
- callback при match-події
- `ccp_compare_irq_handler()` викликається з ISR проєкту

### ISR Integration

Драйвери не створюють ISR.
У реальному ISR потрібно викликати відповідний handler:

- `ccp_capture_irq_handler()`
- `ccp_compare_irq_handler()`

### Приклади

- `drivers/ccp_capture/example.c` — вимірювання ширини імпульсу
- `drivers/ccp_compare/example.c` — таймінгові події (toggle LED)
- `C18/examples/ccp_capture_example.c`
- `XC8/examples/ccp_capture_example.c`
- `C18/examples/ccp_compare_example.c`
- `XC8/examples/ccp_compare_example.c`
