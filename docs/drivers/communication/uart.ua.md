[🇬🇧 English version](./uart.md)

# UART Драйвер

## Опис

UART драйвер надає базову послідовну комунікацію у polling-режимі.
Використовується для передачі байтів/рядків, простого прийому даних і інтеграції з debug-виводом.

## API

- `uart_init(uint32_t baudrate)`
- `uart_write_byte(uint8_t data)`
- `uart_write_string(const char* str)`
- `uart_read_byte(void)`
- `uart_is_data_ready(void)`

## Приклад

```c
#include "core/compiler.h"
#include "drivers/communication/uart/uart.h"

void main(void)
{
    uart_init(9600u);
    uart_write_string("Hello UART\r\n");
}
```

## Особливості

- У загальному прикладі використовується polling (`uart_is_data_ready()`).
- Базовий API не вимагає interrupt-обробки.
- Для `uart_write_string()` рядок має бути нуль-термінований.

## Залежності

- `core/compiler.h`
- `core/types.h`
- `core/debug.h` (опційно, у прикладах)

