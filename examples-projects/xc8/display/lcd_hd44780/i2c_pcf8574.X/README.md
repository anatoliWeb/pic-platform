# i2c_pcf8574 XC8 example

## Purpose

Demonstrate the reusable PCF8574 I2C transport for an HD44780 character LCD
(`libraries/display/lcd_hd44780/lcd_i2c.h`) used directly, without the debug
facade.

## Selected defines

In `project_config.h` and the MPLAB `define-macros`:

| Define | Value |
| --- | ---: |
| `PIC_PLATFORM_CLOCK_HZ` | `8000000` |

The PCF8574 pin mapping (`LCD_I2C_PIN_RS/RW/EN/BL`, `LCD_I2C_DATA_SHIFT`)
defaults to the common backpack layout and can be overridden with compiler
defines. The I2C address is the 7-bit address `0x27`; addresses above `0x7F`
are rejected.

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

This project compiles only `main.c`, `config_bits.c`, `core/delay.c`,
`drivers/communication/i2c/i2c.c` and
`libraries/display/lcd_hd44780/lcd_i2c.c`. It does not compile the debug
facade, UART, GPIO or the parallel LCD driver. The exact source list is in
`nbproject/configurations.xml`.

## Build

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Expected screen

```
LCD I2C OK
PCF8574
```

## Error behavior

`main.c` checks the return value of `lcd_i2c_init()`. If it is not
`LCD_I2C_OK` (backpack missing, wrong address, or bus failure) the example
never writes to the LCD and enters a fail-state loop. An optional status LED
on RB0 blinks while the fail state is active. The failure reason is reported
by `lcd_i2c_last_status()` (`LCD_I2C_NO_ACK`) and `lcd_i2c_is_ready()`
returns 0.
