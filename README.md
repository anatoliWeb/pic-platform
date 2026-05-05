# PIC PLATFORM - Universal Drivers (C18 + XC8)

Бібліотека універсальних драйверів для PIC (фокус: PIC18).

## Підтримувані компілятори

- MPLAB C18
- MPLAB XC8

## Oscillator / Clock

`DRV_XTAL_FREQ` у `core/device.h` є єдиним джерелом частоти для всієї системи.

Чому це важливо:

- delay-логіка використовує цю частоту
- UART baudrate розрахунки залежать від неї
- I2C clock (SSPADD) залежить від неї

Рекомендація:

- не хардкодити частоту в окремих драйверах
- змінювати частоту централізовано тільки в `core/device.h`

### API

- `clock_get_frequency()` повертає compile-time частоту (`DRV_XTAL_FREQ`)

### Oscillator modes (нотатки)

- `HS` — high-speed crystal (високі частоти)
- `XT` — стандартний кварц
- `LP` — low-power crystal
- `RC` — RC-генератор (менш точний)

### Config examples

C18:

```c
#pragma config OSC = HS
```

XC8:

```c
#pragma config FOSC = HS
```

## Інші драйвери

Доступні модулі: GPIO, UART, UART debug, RS485, ADC, PWM, Timer, EEPROM, SPI, I2C, External Interrupt, PORTB Change, WDT, Comparator, CCP Capture/Compare, Reset helper.
