[🇬🇧 English version](./uart_debug.md)

# UART Debug Модуль

## Опис
Опційний макро-базований debug-шар поверх UART з compile-time увімкненням/вимкненням.

## API
- `DBG_PRINT()`
- `DBG_PRINTLN()`
- `DBG_WRITE_BYTE()`
- `DBG_PRINT_INT()`
- `DBG_PRINT_HEX()`
- `dbg_print_int()`
- `dbg_print_hex()`

## Приклад
```c
DBG_PRINT("Start\r\n");
DBG_PRINT_INT(value);
DBG_PRINTLN("");
```

## Особливості
- Вмикається через `DRV_DEBUG_ENABLE=1` і `DRV_USE_UART=1`.
- У вимкненому стані макроси розгортаються в порожні інструкції (zero-cost).

## Залежності
- `core/compiler.h`
- `core/config.h`
- `drivers/uart`
