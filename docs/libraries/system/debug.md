# Debug facade

## Purpose

`libraries/system/debug` is a compile-time debug facade for PIC18F452.
`core/debug.h` forwards to it and keeps the legacy `DBG_*` and `DRV_DEBUG_*`
macros without pulling in `uart_debug.h`.

Backends:

- `UART` - text through the shared UART driver
- `DISPLAY` - text on an LCD 2x16
- `PINS` - GPIO diagnostic signals

Only the selected backend sources and their direct dependencies are compiled.

## Build

Windows build route:

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

Each example builds to its own hex artifact, for example
`dist/default/production/debug_default_uart.X.production.hex`. The exact output
name is derived by MPLAB X from the project folder (`*.X`) and the active
configuration (`default`, `production`).

## Defaults

| Define | Default | Notes |
| --- | ---: | --- |
| `DRV_DEBUG_ENABLE` | `1` | master switch |
| `DRV_DEBUG_BACKEND_UART` | `1` | default backend |
| `DRV_DEBUG_BACKEND_DISPLAY` | `0` | LCD backend |
| `DRV_DEBUG_BACKEND_PINS` | `0` | GPIO backend |
| `DRV_DEBUG_UART_BAUD` | `9600` | UART baud |
| `DRV_DEBUG_DISPLAY_TYPE_LCD_2X16` | `0` | only display type |
| `DRV_DEBUG_DISPLAY_INTERFACE_I2C` | `0` | PCF8574 backpack |
| `DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL` | `0` | 4-bit LCD driver |
| `DRV_DEBUG_DISPLAY_AUTO_INIT` | `1` | init transport/LCD and clear/reset |
| `DRV_DEBUG_PINS_INTERFACE_GPIO` | `0` | direct GPIO pins |
| `DRV_DEBUG_PINS_INTERFACE_I2C` | `0` | rejected at compile time |
| `DRV_DEBUG_PINS_CHANNEL_COUNT` | `4` | 1..8 |
| `DRV_DEBUG_PINS_PULSE_US` | `10000` | must fit `uint16_t` |
| `DRV_DEBUG_LEVEL` | `INFO` | `ERROR`..`TRACE` |

Validation is compile-time only. Boolean defines must be `0` or `1`.

## Behavior

- `DRV_DEBUG_ENABLE=0` turns every `debug_*` macro into a no-op with no argument evaluation.
- Severity-gated macros also compile out without evaluating arguments.
- `debug_write_i16()` handles the full `-32768..32767` range.
- `DRV_DEBUG_DISPLAY_AUTO_INIT=1` initializes transport and LCD, then clears and resets the cursor.
- `DRV_DEBUG_DISPLAY_AUTO_INIT=0` does not initialize transport or LCD and does not clear the external display.
- `DRV_DEBUG_PINS_START_BIT + DRV_DEBUG_PINS_CHANNEL_COUNT` must not exceed `8`.

## Source matrix

| Example | Compiled sources |
| --- | --- |
| `debug_default_uart.X` | `main.c`, `config_bits.c`, `core/delay.c`, `drivers/communication/uart/uart.c`, `libraries/system/debug/debug.c` |
| `debug_display_i2c.X` | `main.c`, `config_bits.c`, `core/delay.c`, `drivers/communication/i2c/i2c.c`, `libraries/system/debug/debug.c` |
| `debug_display_parallel.X` | `main.c`, `config_bits.c`, `core/delay.c`, `drivers/gpio/gpio.c`, `libraries/display/lcd_hd44780/lcd.c`, `libraries/system/debug/debug.c` |
| `debug_pins_gpio.X` | `main.c`, `config_bits.c`, `core/delay.c`, `drivers/gpio/gpio.c`, `libraries/system/debug/debug.c` |
| `debug_multi_backend.X` | `main.c`, `config_bits.c`, `core/delay.c`, `drivers/communication/uart/uart.c`, `drivers/communication/i2c/i2c.c`, `drivers/gpio/gpio.c`, `libraries/system/debug/debug.c` |
| `debug_disabled.X` | `main.c`, `config_bits.c`, `core/delay.c` |

## Quick start examples

Three small projects demonstrate the main debug backends. Each project compiles
only the sources of its selected backend.

### 1. UART

| Item | Value |
| --- | --- |
| Project | `examples-projects/xc8/debug/default_uart.X` |
| Selected defines | `DRV_DEBUG_BACKEND_UART=1` (default backend) |
| Hardware | PIC18F452, RC6/TX pin 25, RC7/RX pin 26, 9600 8N1 |
| Expected | `BOOT` then `counter=` printed every second |

### 2. LCD 2x16 I2C

| Item | Value |
| --- | --- |
| Project | `examples-projects/xc8/debug/display_i2c.X` |
| Selected defines | `DRV_DEBUG_BACKEND_DISPLAY=1`, `DRV_DEBUG_DISPLAY_INTERFACE_I2C=1`, `DRV_DEBUG_DISPLAY_I2C_ADDR=0x27` |
| Hardware | LCD 2x16 + PCF8574, SCL RC3/pin 18, SDA RC4/pin 23, pull-ups |
| Expected | `SYSTEM OK` on row 1, `COUNT=<value>` on row 2 |

### 3. GPIO blink / signals

| Item | Value |
| --- | --- |
| Project | `examples-projects/xc8/debug/pins_gpio.X` |
| Selected defines | `DRV_DEBUG_BACKEND_PINS=1`, `DRV_DEBUG_PINS_INTERFACE_GPIO=1`, `DRV_DEBUG_PINS_PORT=PORTC`, `DRV_DEBUG_PINS_START_BIT=0`, `DRV_DEBUG_PINS_CHANNEL_COUNT=4` |
| Hardware | LED + 330-1000 ohm resistor per channel on PORTC0..3 |
| Expected | pulse, set/clear, code and error patterns on PORTC0..3 |

Additional technical examples: `debug_display_parallel.X`, `debug_multi_backend.X`
(fans text into several textual backends), `debug_disabled.X`
(`DRV_DEBUG_ENABLE=0`, everything compiled out).

## API

- `debug_init()`
- `debug_clear()`
- `debug_write()`, `debug_write_line()`, `debug_write_char()`
- `debug_write_u8()`, `debug_write_u16()`, `debug_write_i16()`
- `debug_write_hex8()`, `debug_write_hex16()`
- `debug_flush()`
- `debug_error()`, `debug_warn()`, `debug_info()`, `debug_trace()`
- `debug_pin_pulse()`, `debug_pin_set()`, `debug_pin_code()`, `debug_pin_error()`

## Compatibility

`core/debug.h` keeps the old macro names (`DBG_PRINT`, `DBG_PRINTLN`,
`DBG_WRITE_BYTE`, `DBG_PRINT_INT`, `DBG_PRINT_HEX`, `DRV_DEBUG_INIT`,
`DRV_DEBUG_LOG`, `DRV_DEBUG_LOG_HEX`) and maps them to the new facade.

`libraries/system/uart_debug/uart_debug.h` remains available for older code
that includes it directly.

## ROM/RAM

| Config | ROM | RAM |
| --- | ---: | ---: |
| `debug_default_uart.X` | `1246` | `93` |
| `debug_display_i2c.X` | `2132` | `32` |
| `debug_display_parallel.X` | `2234` | `50` |
| `debug_pins_gpio.X` | `976` | `23` |
| `debug_multi_backend.X` | `3028` | `98` |
| `debug_disabled.X` | `102` | `3` |
