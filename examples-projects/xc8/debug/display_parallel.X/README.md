# debug_display_parallel XC8 example

## Purpose

Show the `DISPLAY` backend using the LCD 2x16 parallel (4-bit) adapter as an
additional variant of the main I2C display example.

## Selected defines

In `project_config.h`:

| Define | Value |
| --- | ---: |
| `DRV_DEBUG_BACKEND_UART` | `0` |
| `DRV_DEBUG_BACKEND_DISPLAY` | `1` |
| `DRV_DEBUG_DISPLAY_TYPE_LCD_2X16` | `1` |
| `DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL` | `1` |

## Hardware and pin mapping

- PIC18F452
- LCD 2x16 (HD44780) in 4-bit mode
- Data bus on PORTD (`lcd_init_pins` uses `RD4..RD7`)
- RS on RB0
- EN on RB1

The data nibble is `D4..D7`, so the driver writes only the upper four data
lines (`RD4..RD7`).

## Display ownership

The parallel driver needs its pin pointers before use. The project calls
`lcd_prepare()` (which calls `lcd_init_pins(...)`) **before** `debug_init()`.
The example owns the `PORTD` data bus and the `RB0`/`RB1` control lines.

## Source isolation

This project compiles only the parallel LCD path (`gpio.c`, `lcd.c`). It does
not compile `i2c.c` or the I2C backpack adapter. The exact source list is in
`nbproject/configurations.xml`.

## Build

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Expected result

The LCD shows a boot message and a `tick=` counter on two lines.
