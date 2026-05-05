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

`core/` — це базовий шар, від якого залежать усі драйвери.
Він надає спільні типи, макроси, конфігурацію та компіляторну абстракцію.

Файли Core Layer:

- `core/compiler.h` - абстракція C18/XC8 і базові compiler wrappers
- `core/types.h` - спільні типи статусів та булевих значень
- `core/bit_utils.h` - універсальні бітові макроси
- `core/delay.h` - єдині wrappers для затримок
- `core/device.h` - базові device/clock налаштування
- `core/interrupts.h` - helper-макроси для global interrupts
- `core/config.h` - compile-time feature flags

Як використовуються драйверами:

- драйвери підключають `core/compiler.h` для compiler portability
- за потреби підключають `core/types.h`, `core/bit_utils.h`, `core/delay.h`
- project-level налаштування виконуються через `core/config.h` та `core/device.h`

## GPIO Driver

GPIO — еталонний універсальний драйвер для керування напрямком та станом пінів через регістри і номер біта.
Драйвер не хардкодить конкретні піни і працює через `volatile uint8_t*`.

Базовий приклад використання:

```c
gpio_set_output(&TRISB, 0u);
gpio_write_high(&PORTB, 0u);
if (gpio_read(&PORTB, 1u) != 0u)
{
    gpio_toggle(&PORTB, 0u);
}
```

API:

- `void gpio_init(void);`
- `void gpio_set_output(volatile uint8_t* tris, uint8_t bit);`
- `void gpio_set_input(volatile uint8_t* tris, uint8_t bit);`
- `void gpio_write_high(volatile uint8_t* port, uint8_t bit);`
- `void gpio_write_low(volatile uint8_t* port, uint8_t bit);`
- `void gpio_toggle(volatile uint8_t* port, uint8_t bit);`
- `uint8_t gpio_read(volatile uint8_t* port, uint8_t bit);`

### GPIO Driver Architecture

- `drivers/gpio/gpio.c` — universal entry point
- `C18/drivers/gpio/gpio.c` — C18-specific implementation
- `XC8/drivers/gpio/gpio.c` — XC8-specific implementation

Fallback logic у `drivers/gpio/gpio.c`:

- для `DRV_COMPILER_C18` підключається `C18` реалізація
- для `DRV_COMPILER_XC8` підключається `XC8` реалізація
- інакше використовується вбудований universal fallback

## UART Driver

UART — легкий універсальний драйвер із єдиним API для всіх компіляторів.
Підтримує polling обмін та мінімальний RX circular buffer без dynamic memory.

API:

- `void uart_init(uint32_t baudrate);`
- `void uart_write_byte(uint8_t data);`
- `void uart_write_string(const char* str);`
- `uint8_t uart_read_byte(void);`
- `uint8_t uart_is_data_ready(void);`

Debug note:

- при `DRV_DEBUG_ENABLE=1` макрос `DBG_PRINT(str)` у `core/debug.h` використовує `uart_write_string(str)`.

### UART Driver Architecture

- `drivers/uart/uart.c` — universal entry point
- `C18/drivers/uart/uart.c` — C18-specific implementation
- `XC8/drivers/uart/uart.c` — XC8-specific implementation

Fallback logic у `drivers/uart/uart.c`:

- для `DRV_COMPILER_C18` підключається `C18` реалізація
- для `DRV_COMPILER_XC8` підключається `XC8` реалізація
- інакше використовується вбудований universal fallback

## Підтримувані компілятори

- MPLAB C18
- MPLAB XC8
