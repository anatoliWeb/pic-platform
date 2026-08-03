# debug_multi_backend XC8 example

## Purpose

Technical example that drives `UART + DISPLAY + PINS` at the same time from one
debug facade.

## Selected defines

In MPLAB `define-macros`:

| Define | Value |
| --- | ---: |
| `DRV_DEBUG_BACKEND_UART` | `1` (default) |
| `DRV_DEBUG_BACKEND_DISPLAY` | `1` |
| `DRV_DEBUG_BACKEND_PINS` | `1` |
| `DRV_DEBUG_DISPLAY_TYPE_LCD_2X16` | `1` |
| `DRV_DEBUG_DISPLAY_INTERFACE_I2C` | `1` |
| `DRV_DEBUG_DISPLAY_I2C_ADDR` | `0x27` |
| `DRV_DEBUG_PINS_INTERFACE_GPIO` | `1` |
| `DRV_DEBUG_PINS_PORT` | `PORTC` |
| `DRV_DEBUG_PINS_TRIS` | `TRISC` |
| `DRV_DEBUG_PINS_START_BIT` | `0` |
| `DRV_DEBUG_PINS_CHANNEL_COUNT` | `4` |

## Text API vs signal API

- **Text API** (`debug_write`, `debug_write_line`, `debug_write_u16`, ...) is
  sent to every enabled textual backend: here UART **and** the LCD. One textual
  call appears on both.
- **Signal API** (`debug_pin_*`) is routed only to the PINS backend. Signal
  calls never touch UART or the LCD.

This is the key point of the multi-backend example: text fans out, signals go
only to the pins.

## Hardware

- PIC18F452
- UART TX on RC6 / pin 25
- LCD 2x16 I2C backpack at `0x27`
- GPIO channels on PORTC bits 0..3

## Source isolation

This project compiles `uart.c`, `i2c.c` and `gpio.c` together. The exact source
list is in `nbproject/configurations.xml`.

## Build

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Expected result

Text goes to UART and the LCD at the same time, while channel 0 emits
heartbeat pulses.
