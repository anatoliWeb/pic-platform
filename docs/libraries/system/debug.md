# Debug facade

## Purpose

`libraries/system/debug` is an extensible, compile-time debug facade for
PIC18F452. Application code calls one small API; the actual output is routed
to the backends selected by configuration defines:

```text
UART     - textual output to the shared UART driver
DISPLAY  - textual output on a character LCD
PINS     - non-textual diagnostic signals on GPIO outputs
```

Only the code for the selected backends and their dependencies is compiled,
so a UART-only build never pulls in I2C, LCD or GPIO debug code.

Entry point: `core/debug.h` (kept backward compatible) forwards to the module.

## Global enable / disable

The single master switch is `DRV_DEBUG_ENABLE` (default `1`, see
`core/config.h`). When it is `0`:

- no backend is compiled;
- UART/display/I2C/GPIO debug dependencies are not pulled in;
- every `debug_*` call becomes a preprocessor no-op;
- the application can keep calling the API without extra `#if` guards.

## Backend selection

```c
#define DRV_DEBUG_BACKEND_UART     1   /* default */
#define DRV_DEBUG_BACKEND_DISPLAY  0   /* default */
#define DRV_DEBUG_BACKEND_PINS     0   /* default */
```

Without any overrides the facade uses the UART backend at the default baud.
More than one backend can be enabled at the same time; a textual message is
then sent to every enabled textual backend.

## Configuration defines

| Define | Default | Meaning |
| --- | ---: | --- |
| `DRV_DEBUG_ENABLE` | `1` | master on/off |
| `DRV_DEBUG_BACKEND_UART` | `1` | enable UART backend |
| `DRV_DEBUG_BACKEND_DISPLAY` | `0` | enable DISPLAY backend |
| `DRV_DEBUG_BACKEND_PINS` | `0` | enable PINS backend |
| `DRV_DEBUG_UART_BAUD` | `9600` | UART baudrate |
| `DRV_DEBUG_DISPLAY_TYPE_LCD_2X16` | `0` | LCD 2x16 display adapter |
| `DRV_DEBUG_DISPLAY_INTERFACE_I2C` | `0` | use I2C PCF8574 backpack |
| `DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL` | `0` | use direct 4-bit lcd driver |
| `DRV_DEBUG_DISPLAY_I2C_ADDR` | - | I2C backpack address (I2C mode) |
| `DRV_DEBUG_DISPLAY_I2C_FREQ` | `100000` | I2C clock in Hz (I2C mode) |
| `DRV_DEBUG_DISPLAY_AUTO_INIT` | `1` | let debug init the display |
| `DRV_DEBUG_PINS_INTERFACE_GPIO` | `0` | drive pins via GPIO driver |
| `DRV_DEBUG_PINS_INTERFACE_I2C` | `0` | not supported (no expander) |
| `DRV_DEBUG_PINS_PORT` | - | output port register (GPIO mode) |
| `DRV_DEBUG_PINS_TRIS` | - | direction register (GPIO mode) |
| `DRV_DEBUG_PINS_START_BIT` | - | first channel bit (GPIO mode) |
| `DRV_DEBUG_PINS_CHANNEL_COUNT` | `4` | number of channels (GPIO mode) |
| `DRV_DEBUG_PINS_PULSE_US` | `10000` | pulse width in microseconds |
| `DRV_DEBUG_LEVEL` | `INFO` | severity filter |

All defaults live in `debug.h` unless noted otherwise, so a project can
override each with a `#define` before including the facade.

## Minimum UART integration

With no extra `#define` the facade uses the UART backend at `9600` baud:

```c
#include "core/debug.h"

int main(void)
{
    debug_init();
    debug_write_line("BOOT");
    for (;;) { }
}
```

## UART settings

- Default baud: `9600` (matches the current UART drivers and examples).
- Override with `DRV_DEBUG_UART_BAUD`.
- PIC18F452 has a single EUSART; the UART instance is implicit.
- `DRV_DEBUG_BACKEND_UART` must be `1` for the UART backend to compile.

## DISPLAY behavior

The facade tracks the cursor on `LCD_TYPE_LCD_2X16` (2 rows x 16 columns).
`\n` moves to the next row and wraps after row 2; a row fills to 16 columns
and then wraps. Long text is therefore truncated by row/column wrap, not
scrolled. `debug_clear()` clears the display.

Ownership modes:

- `DRV_DEBUG_DISPLAY_AUTO_INIT=1`: `debug_init()` initializes the display
  hardware (and the I2C master in I2C mode).
- `DRV_DEBUG_DISPLAY_AUTO_INIT=0`: the display must be initialized by
  external code; debug only uses it.

### I2C transport

`DRV_DEBUG_DISPLAY_INTERFACE_I2C = 1` uses the shared I2C master and a
PCF8574 backpack. `DRV_DEBUG_DISPLAY_I2C_ADDR` is required (e.g. `0x27`).
Only the I2C code is compiled; parallel GPIO display code is not.

### Direct parallel transport

`DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL = 1` uses
`libraries/display/lcd_hd44780` in 4-bit mode. Because that driver needs its
pin pointers, the project must call `lcd_init_pins(...)` before
`debug_init()`. Only the parallel code is compiled; I2C transport is not.

## PINS backend

The PINS backend is not textual. It drives diagnostic signals on GPIO
outputs. Supported operations:

- `debug_pin_pulse(channel)` - one pulse on a channel
- `debug_pin_set(channel, state)` - set/clear a channel
- `debug_pin_code(value)` - write a bit pattern across all channels
- `debug_pin_error(code)` - blink an error code on channel 0

Direct GPIO mode (`DRV_DEBUG_PINS_INTERFACE_GPIO=1`) uses the GPIO driver on a
contiguous range of bits `DRV_DEBUG_PINS_START_BIT .. +CHANNEL_COUNT` of
`DRV_DEBUG_PINS_PORT`/`DRV_DEBUG_PINS_TRIS`. I2C expander mode is not
implemented in pic-platform and is rejected at compile time.

## Resource ownership

- UART pins/port: project-owned (commonly `RC6/TX`, `RC7/RX`).
- LCD pins: project-defined in parallel mode; I2C pins belong to the I2C
  master and are debug-owned only in auto-init mode.
- GPIO debug pins: exclusively used by the PINS backend when enabled.

## ROM/RAM impact

With no debug (`DRV_DEBUG_ENABLE=0`) the module contributes nothing. Enabled
UART adds the UART driver and the ~labin on top of an already used UART. The
DISPLAY backend adds the LCD/I2C adapter and its driver. The PINS backend only
adds a few GPIO calls. No heap/allocation, no function-pointer tables, no
recursion.

## Blocking behavior

UART writes are blocking (driver FIFO to TX). I2C and parallel LCD writes are
blocking functions with short delays; large batches on an LCD take time and
delay the caller.

## Backward compatibility

The old UART debug macros are kept:

| Old API | New API | Compatibility |
| --- | --- | --- |
| `DBG_PRINT(s)` | `debug_write(s)` | retained |
| `DBG_PRINTLN(s)` | `debug_write_line(s)` | retained |
| `DBG_WRITE_BYTE(b)` | `debug_write_char(c)` | retained |
| `DBG_PRINT_INT(v)` | `debug_write_u16/u16` | retained |
| `DBG_PRINT_HEX(v)` | `debug_write_hex8` | retained |
| `dbg_print_int()` / `dbg_print_hex8()` | kept under `uart_debug` | retained |
| `DRV_DEBUG_INIT()` | alias to `debug_init()` | retained |
| `DRV_DEBUG_LOG` / `DRV_DEBUG_LOG_HEX` | alias to facade | retained |

`core/debug.h` and `libraries/system/uart_debug/uart_debug.h` are unchanged so
existing projects keep building.

## Adding a new backend

1. Create `debug_backend_<name>.{h,c}` with `debug_<name>_init/...` functions.
2. Add a `DRV_DEBUG_BACKEND_<NAME>` define, default off.
3. In `debug.c`, include the backend source under `#if DRV_DEBUG_BACKEND_<NAME>`
   and dispatch from the facade functions.
4. Add compile-time validation in `debug.h`.

## Adding a new display adapter

1. Add a `DRV_DEBUG_DISPLAY_TYPE_<X>` define.
2. Create `debug_display_<x>.{h,c}` implementing
   `debug_lcd_init/clear/set_cursor/write_char`.
3. Wire it in `debug_backend_display.c` under the new type define.

## Adding a new pins transport

1. Add a `DRV_DEBUG_PINS_INTERFACE_<X>` define.
2. Create `debug_pins_<x>.{h,c}` implementing
   `debug_gpio_pins_init/set/pulse/code/error`.
3. Dispatch it from `debug_backend_pins.c` and validate it in `debug.h`.