# debug

## Purpose

Extensible compile-time debug facade for PIC18F452. Routes diagnostic output
to one or more backends (UART, DISPLAY, PINS) selected with configuration
defines. The facade never knows transport details; only selected backends and
their dependencies are compiled into the firmware.

## Location

```text
libraries/system/debug/debug.h
libraries/system/debug/debug.c
libraries/system/debug/debug_backend_uart.h
libraries/system/debug/debug_backend_uart.c
libraries/system/debug/debug_backend_display.h
libraries/system/debug/debug_backend_display.c
libraries/system/debug/debug_backend_pins.h
libraries/system/debug/debug_backend_pins.c
libraries/system/debug/debug_display_lcd_2x16.h
libraries/system/debug/debug_display_lcd_2x16.c
libraries/system/debug/debug_pins_gpio.h
libraries/system/debug/debug_pins_gpio.c
core/debug.h
docs/libraries/system/debug.md
docs/libraries/system/debug.ua.md
```

Implementation note: `debug.c` pulls in the backend and adapter `.c` files for
only the enabled backends, so unselected transports are never compiled.

## Use when

- you want a facade that can target UART, a character display, or GPIO pins;
- you want compile-time backend selection without transport boilerplate in
  application code;
- you want the same debug API to later gain new backends without rewriting
  callers.

## Do not use when

- you only need plain UART debug output — `uart_debug` is lighter;
- you need a transport that has no adapter or driver in the repository
  (for example an I2C GPIO expander, SPI debugger, or memory logger).

## Public API

| Function | Purpose |
| --- | --- |
| `debug_init()` | init configured backends (UART baud, display init) |
| `debug_clear()` | clear output where supported (display) |
| `debug_write(s)` | write text, no newline |
| `debug_write_line(s)` | write text + newline |
| `debug_write_char(c)` | write one byte |
| `debug_write_u8()/u16()/i16()` | write decimal value |
| `debug_write_hex8()/hex16()` | write hex digits |
| `debug_flush()` | flush pending output (no-op today) |
| `debug_error()/warn()/info()/trace()` | severity-gated text macros |
| `debug_pin_pulse/set/code/error()` | PINS backend signals |

When disabled, all of the above are preprocessor no-ops.

## Configuration defines

Global master switch (default `1`): `DRV_DEBUG_ENABLE`.

Backends: `DRV_DEBUG_BACKEND_UART` (default `1`), `DRV_DEBUG_BACKEND_DISPLAY`,
`DRV_DEBUG_BACKEND_PINS` (both default `0`). Enable any one or several.

See `docs/libraries/system/debug.md` for the full define reference, solve and
resource ownership.

## Required files

```text
libraries/system/debug/debug.h
libraries/system/debug/debug.c
```

The matching backend carried by `debug.c` are listed in the module files.

## Dependencies

```text
core/compiler.h
core/config.h
core/types.h
core/delay.h
drivers/communication/uart/uart.h
drivers/communication/i2c/i2c.h
drivers/gpio/gpio.h
libraries/display/lcd_hd44780/lcd.h
```

Dependencies are compile-time gated: UART driver only with the UART backend,
I2C/LCD only with the display backend, GPIO only with the pins backend.

## Source inclusion strategy

Single shared implementation: add `debug.c`; it compiles in only the enabled
backend/ adapter sources. No XC8/C18 copies are needed because the underlying
drivers provide their own dispatchers.

## Human documentation

```text
docs/libraries/system/debug.md
docs/libraries/system/debug.ua.md
```

## Known limitations

- Only LCD 2x16 is currently exposed as a display type; other drivers exist
  in the repository but have no debug adapter yet.
- The direct-parallel LCD transport requires the project to call
  `lcd_init_pins()` before `debug_init()`.
- PINS supports direct GPIO only; an I2C expander transport is not
  implemented (no expander library in pic-platform).

## Extension points

- Add a backend: create `debug_backend_<name>.[ch]`, dispatch it from
  `debug.c`, expose a select `DRV_DEBUG_BACKEND_<NAME>` define, and update the
  config validation in `debug.h`.
- Add a display type: add a `debug_display_<type>.[ch]` adapter and a
  `DRV_DEBUG_DISPLAY_TYPE_*` define.
- Add a pins transport: add a `debug_pins_<transport>.[ch]` adapter and a
  `DRV_DEBUG_PINS_INTERFACE_*` define.

## AI decision rule

Use `debug` when output must reach more than one transport or when the
transport is chosen at compile time. Prefer `uart_debug` for pure UART logs.