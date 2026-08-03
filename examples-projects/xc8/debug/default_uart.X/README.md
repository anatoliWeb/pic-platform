# debug_default_uart XC8 example

## Purpose

Demonstrate the `UART` backend of the debug facade (`core/debug.h`) as the
default backend. Backend selection stays at the library default.

## Selected defines

Default values from `libraries/system/debug/debug.h` apply:

| Define | Value |
| --- | ---: |
| `DRV_DEBUG_BACKEND_UART` | `1` (default) |
| `DRV_DEBUG_BACKEND_DISPLAY` | `0` (default) |
| `DRV_DEBUG_BACKEND_PINS` | `0` (default) |
| `DRV_DEBUG_UART_BAUD` | `9600` |

## Hardware

- PIC18F452
- UART TX on RC6 / pin 25
- UART RX on RC7 / pin 26
- Virtual Terminal at 9600 baud, 8N1

## Source isolation

This project compiles only the UART backend. It does not compile I2C, LCD,
the display backend, or the GPIO pins backend. The exact source list is in
`nbproject/configurations.xml`.

## Build

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Expected result

`BOOT` appears once, then a `counter=` value is printed every second.
