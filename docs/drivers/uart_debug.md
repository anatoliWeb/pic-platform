[🇺🇦 Ukrainian version](./uart_debug.ua.md)

# UART Debug Module

## Description
Optional macro-based debug layer over UART with compile-time enable/disable.

## API
- `DBG_PRINT()`
- `DBG_PRINTLN()`
- `DBG_WRITE_BYTE()`
- `DBG_PRINT_INT()`
- `DBG_PRINT_HEX()`
- `dbg_print_int()`
- `dbg_print_hex()`

## Example
```c
DBG_PRINT("Start\r\n");
DBG_PRINT_INT(value);
DBG_PRINTLN("");
```

## Notes
- Enabled by `DRV_DEBUG_ENABLE=1` and `DRV_USE_UART=1`.
- When disabled, macros expand to empty statements (zero-cost concept).

## Dependencies
- `core/compiler.h`
- `core/config.h`
- `drivers/uart`
