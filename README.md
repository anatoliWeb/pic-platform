# PIC PLATFORM - Universal Drivers (C18 + XC8)

Це база універсальних драйверів для PIC (фокус: PIC18).
Репозиторій не є application-проєктом.

## Цілі

- Універсальні перевикористовувані драйвери
- Підтримка MPLAB C18 та MPLAB XC8
- Мінімальна залежність драйверів від компілятора
- Усі compiler-specific відмінності винесені в `core/compiler.h`

## Структура

- `core/`
- `drivers/`
- `examples/`
- `docs/`
- `C18/`
- `XC8/`

## Як створити новий драйвер

1. Прочитати `DRIVER_GUIDELINES.md`
2. Скопіювати `/drivers/_template`
3. Перейменувати під назву драйвера
4. Реалізувати API та внутрішню логіку драйвера
5. Додати/адаптувати `example.c`

## Приклади для компіляторів

- `examples/common` - універсальні приклади
- `C18/examples` - приклади під MPLAB C18
- `XC8/examples` - приклади під MPLAB XC8

## Core Layer

Базовий шар з типами, macro utilities і compiler abstraction.

## GPIO Driver

### GPIO Driver Architecture

- `drivers/gpio/gpio.c` — universal entry point
- `C18/drivers/gpio/gpio.c` — C18-specific implementation
- `XC8/drivers/gpio/gpio.c` — XC8-specific implementation

## UART Driver

### UART Driver Architecture

- `drivers/uart/uart.c` — universal entry point
- `C18/drivers/uart/uart.c` — C18-specific implementation
- `XC8/drivers/uart/uart.c` — XC8-specific implementation

## UART Debug Module

Опціональний debug layer поверх UART через macro API.

## RS485 Driver

UART-based layer with direction control, frame, CRC8 and timeout.

## ADC Driver

ADC driver для sensors/buttons/voltage з averaging і helper-функціями.

## PWM Driver

PWM реалізовано через CCP1/CCP2 з базою на Timer2.

Ключова формула частоти:

- `PWM Frequency = Fosc / (4 * prescaler * (PR2 + 1))`

Можливості:

- підтримка `PWM_CHANNEL_1` і `PWM_CHANNEL_2`
- `pwm_init(frequency)` налаштовує Timer2, PR2 та prescaler
- `pwm_set_duty(channel, duty)` керує 10-bit duty (`CCPRxL` + `DCxB`)
- `pwm_start` / `pwm_stop`

### PWM Driver Architecture

- `drivers/pwm/pwm.c` — universal entry point + fallback
- `C18/drivers/pwm/pwm.c` — C18-specific implementation
- `XC8/drivers/pwm/pwm.c` — XC8-specific implementation

Приклади використання:

- LED dimming (sweep duty)
- Fan low/medium/high speed presets

## Підтримувані компілятори

- MPLAB C18
- MPLAB XC8
