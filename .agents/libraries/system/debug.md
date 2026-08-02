# debug

## Purpose

Compile-time debug facade for PIC18F452. `core/debug.h` forwards to this
module and keeps the legacy macros without forcing a `uart_debug.h` include.

## Location

```text
libraries/system/debug/debug.h
libraries/system/debug/debug.c
libraries/system/debug/debug_backend_uart.h
libraries/system/debug/debug_backend_display.h
libraries/system/debug/debug_backend_pins.h
libraries/system/debug/debug_display_lcd_2x16.h
libraries/system/debug/debug_pins_gpio.h
libraries/system/debug/debug_backend_uart.c
libraries/system/debug/debug_backend_display.c
libraries/system/debug/debug_backend_pins.c
libraries/system/debug/debug_display_lcd_2x16.c
libraries/system/debug/debug_pins_gpio.c
core/debug.h
docs/libraries/system/debug.md
docs/libraries/system/debug.ua.md
```

## Use when

- you need UART, DISPLAY, or PINS debug from one API;
- backend choice must be compile-time only;
- you want one facade that can fan out to more than one backend.

## Do not use when

- you only need plain UART debug and want the smallest route possible;
- you need a transport that is not in this repository.

## Source matrix

- `debug_default_uart.X`: `main.c`, `config_bits.c`, `core/delay.c`, `drivers/communication/uart/uart.c`, `libraries/system/debug/debug.c`
- `debug_display_i2c.X`: `main.c`, `config_bits.c`, `core/delay.c`, `drivers/communication/i2c/i2c.c`, `libraries/system/debug/debug.c`
- `debug_display_parallel.X`: `main.c`, `config_bits.c`, `core/delay.c`, `drivers/gpio/gpio.c`, `libraries/display/lcd_hd44780/lcd.c`, `libraries/system/debug/debug.c`
- `debug_pins_gpio.X`: `main.c`, `config_bits.c`, `core/delay.c`, `drivers/gpio/gpio.c`, `libraries/system/debug/debug.c`
- `debug_multi_backend.X`: `main.c`, `config_bits.c`, `core/delay.c`, `drivers/communication/uart/uart.c`, `drivers/communication/i2c/i2c.c`, `drivers/gpio/gpio.c`, `libraries/system/debug/debug.c`
- `debug_disabled.X`: `main.c`, `config_bits.c`, `core/delay.c`

## Public API

`debug_init`, `debug_clear`, `debug_write`, `debug_write_line`,
`debug_write_char`, `debug_write_u8`, `debug_write_u16`, `debug_write_i16`,
`debug_write_hex8`, `debug_write_hex16`, `debug_flush`, `debug_error`,
`debug_warn`, `debug_info`, `debug_trace`, `debug_pin_pulse`, `debug_pin_set`,
`debug_pin_code`, `debug_pin_error`.

Filtered and disabled macros do not evaluate arguments.

## Configuration

- `DRV_DEBUG_ENABLE` default `1`.
- `DRV_DEBUG_BACKEND_UART` default `1`; `DISPLAY` and `PINS` default `0`.
- `DRV_DEBUG_UART_BAUD` default `9600`.
- `DRV_DEBUG_DISPLAY_AUTO_INIT=1` initializes transport and LCD, then clears and resets.
- `DRV_DEBUG_DISPLAY_AUTO_INIT=0` leaves external display ownership alone.
- `DRV_DEBUG_PINS_START_BIT + DRV_DEBUG_PINS_CHANNEL_COUNT <= 8`.
- `DRV_DEBUG_PINS_PULSE_US` must fit in `uint16_t`.
- Boolean defines must be `0` or `1`.

## Compatibility

Legacy macros kept in `core/debug.h`: `DBG_PRINT`, `DBG_PRINTLN`,
`DBG_WRITE_BYTE`, `DBG_PRINT_INT`, `DBG_PRINT_HEX`, `DRV_DEBUG_INIT`,
`DRV_DEBUG_LOG`, `DRV_DEBUG_LOG_HEX`.

Direct `libraries/system/uart_debug/uart_debug.h` include remains supported for older projects.

## ROM/RAM

`debug_default_uart.X` 1246/93
`debug_display_i2c.X` 2132/32
`debug_display_parallel.X` 2234/50
`debug_pins_gpio.X` 976/23
`debug_multi_backend.X` 3028/98
`debug_disabled.X` 102/3
