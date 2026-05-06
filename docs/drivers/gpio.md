[🇺🇦 Ukrainian version](./gpio.ua.md)

# GPIO Driver

## Description

The GPIO driver provides generic digital pin control through register pointers and bit indexes.
It is used for LED control, button reading, and simple digital I/O tasks without hardcoded pins.

## API

- `gpio_init()`
- `gpio_set_output(volatile uint8_t* tris, uint8_t bit)`
- `gpio_set_input(volatile uint8_t* tris, uint8_t bit)`
- `gpio_write_high(volatile uint8_t* port, uint8_t bit)`
- `gpio_write_low(volatile uint8_t* port, uint8_t bit)`
- `gpio_toggle(volatile uint8_t* port, uint8_t bit)`
- `gpio_read(volatile uint8_t* port, uint8_t bit)`

## Example

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

## Notes

- Polling-style usage in the common example.
- No interrupt handling is implemented in this driver.
- Pin mapping is external: pass `TRIS`/`PORT` pointers and bit index from the project.

## Dependencies

- `core/compiler.h`
- `core/types.h`
- `core/bit_utils.h` (used by implementation)

