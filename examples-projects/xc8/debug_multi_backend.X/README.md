# debug_multi_backend XC8 example

## Purpose

Show combined `UART + DISPLAY + PINS` debug output.

## Hardware

- PIC18F452
- UART TX on RC6 / pin 25
- LCD 2x16 I2C backpack at `0x27`
- GPIO channels on PORTC bits 0..3

## Build

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Expected result

Text goes to UART and LCD, while channel 0 emits heartbeat pulses.
