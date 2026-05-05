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

## PORTB Change Interrupt

Драйвер підтримує переривання по зміні на RB4–RB7.

Що робить драйвер:

- init RB4..RB7 як input
- робить dummy read `PORTB` для скидання mismatch
- керує `RBIE` enable/disable
- тримає `last_state` та `changed_mask`
- викликає callback: `(changed_mask, current_state)`

Формування маски змін:

- `changed_mask = (current ^ last_state) & 0xF0`

### ISR Integration

Драйвер не створює ISR.
У вашому ISR потрібно викликати:

`portb_change_irq_handler();`

### PORTB Change Architecture

- `drivers/portb_change/portb_change.c` — universal entrypoint + fallback
- `C18/drivers/portb_change/portb_change.c` — C18-specific implementation
- `XC8/drivers/portb_change/portb_change.c` — XC8-specific implementation

### Examples

- `drivers/portb_change/example.c` — button/keypad style callback pattern
- `C18/examples/portb_change_example.c`
- `XC8/examples/portb_change_example.c`

## Інші драйвери

Доступні базові драйвери: GPIO, UART, UART debug, RS485, ADC, PWM, Timer, EEPROM, SPI, I2C, external interrupt.
