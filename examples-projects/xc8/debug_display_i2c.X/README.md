# debug_display_i2c XC8 example

## Purpose

Demonstrate the `DISPLAY` backend of the debug facade (`core/debug.h`) on an
LCD 2x16 with a PCF8574 I2C backpack.

## Selected defines

In `project_config.h`:

| Define | Value |
| --- | ---: |
| `DRV_DEBUG_BACKEND_UART` | `0` |
| `DRV_DEBUG_BACKEND_DISPLAY` | `1` |
| `DRV_DEBUG_BACKEND_PINS` | `0` |
| `DRV_DEBUG_DISPLAY_TYPE_LCD_2X16` | `1` |
| `DRV_DEBUG_DISPLAY_INTERFACE_I2C` | `1` |
| `DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL` | `0` |
| `DRV_DEBUG_DISPLAY_I2C_ADDR` | `0x27` |

## Hardware

- PIC18F452
- LCD 2x16 (HD44780) + PCF8574 I2C backpack
- VCC
- GND
- SDA on RC4 / pin 23
- SCL on RC3 / pin 18
- I2C address `0x27`

## Wiring

| LCD / backpack | PIC18F452 | PIC physical pin |
| --- | --- | ---: |
| VCC | VDD | 11, 32 |
| GND | VSS | 12, 31 |
| SDA | RC4 | 23 |
| SCL | RC3 | 18 |

Use pull-up resistors (4.7k) on SDA and SCL.

## Source isolation

This project compiles only the I2C display backend. It does not compile
`uart.c`, the UART backend, the GPIO pins backend, or the parallel LCD driver.
The exact source list is in `nbproject/configurations.xml`.

## Build

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Expected screen

```
SYSTEM OK
COUNT=<value>
```

The screen is cleared, `SYSTEM OK` appears on row 1 and `COUNT=` with the
counter value on row 2.
