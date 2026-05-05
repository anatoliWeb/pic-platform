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

## External Interrupt Driver

Драйвер підтримує зовнішні переривання:

- INT0
- INT1
- INT2

Можливості:

- enable / disable кожного каналу
- edge config: falling / rising
- callback для кожного каналу
- перевірка та очистка interrupt flag
- єдиний `ext_interrupt_irq_handler()`

### ISR Integration

Драйвер не створює реальний ISR.
Реальний ISR у вашому проєкті має викликати:

`ext_interrupt_irq_handler();`

### External Interrupt Architecture

- `drivers/ext_interrupt/ext_interrupt.c` — universal entrypoint + fallback
- `C18/drivers/ext_interrupt/ext_interrupt.c` — C18-specific implementation
- `XC8/drivers/ext_interrupt/ext_interrupt.c` — XC8-specific implementation

### Button Interrupt Example

- налаштувати edge (наприклад falling)
- призначити callback
- увімкнути канал
- у callback змінити flag або toggle LED

## Інші драйвери

Доступні базові драйвери: GPIO, UART, UART debug, RS485, ADC, PWM, Timer, EEPROM, SPI, I2C.
