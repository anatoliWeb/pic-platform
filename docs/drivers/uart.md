[🇺🇦 Ukrainian version](./uart.ua.md)

# UART Driver

## Description

The UART driver provides basic serial communication in polling mode.
It is used for byte/string transmission, simple receive handling, and debug output integration.

## API

- `uart_init(uint32_t baudrate)`
- `uart_write_byte(uint8_t data)`
- `uart_write_string(const char* str)`
- `uart_read_byte(void)`
- `uart_is_data_ready(void)`

## Example

```c
#include "core/compiler.h"
#include "drivers/uart/uart.h"

void main(void)
{
    uart_init(9600u);
    uart_write_string("Hello UART\r\n");
}
```

## Notes

- The common example uses polling (`uart_is_data_ready()`).
- No mandatory interrupt flow in base API.
- Keep strings zero-terminated for `uart_write_string()`.

## Dependencies

- `core/compiler.h`
- `core/types.h`
- `core/debug.h` (optional, in examples)

