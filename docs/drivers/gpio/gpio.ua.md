[🇬🇧 English version](./gpio.md)

# GPIO Драйвер

## Опис

GPIO драйвер надає універсальне керування цифровими пінaми через вказівники на регістри та номер біта.
Використовується для керування LED, читання кнопок і базових задач цифрового вводу/виводу без хардкоду пінів.

## API

- `gpio_init()`
- `gpio_set_output(volatile uint8_t* tris, uint8_t bit)`
- `gpio_set_input(volatile uint8_t* tris, uint8_t bit)`
- `gpio_write_high(volatile uint8_t* port, uint8_t bit)`
- `gpio_write_low(volatile uint8_t* port, uint8_t bit)`
- `gpio_toggle(volatile uint8_t* port, uint8_t bit)`
- `gpio_read(volatile uint8_t* port, uint8_t bit)`

## Приклад

```c
#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/gpio/gpio.h"

void main(void)
{
    volatile uint8_t* led_tris = &TRISB;
    volatile uint8_t* led_port = &PORTB;

    gpio_init();
    gpio_set_output(led_tris, 0u);

    while (1)
    {
        gpio_toggle(led_port, 0u);
        DRV_DELAY_MS(200);
    }
}
```

## Особливості

- У загальному прикладі використовується polling-модель.
- Обробка interrupt у цьому драйвері не реалізована.
- Прив'язка пінів задається ззовні: через `TRIS`/`PORT` і номер біта.

## Залежності

- `core/compiler.h`
- `core/types.h`
- `core/bit_utils.h` (використовується в реалізації)

