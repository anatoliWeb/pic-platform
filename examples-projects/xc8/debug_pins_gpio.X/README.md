# debug_pins_gpio XC8 example

## Purpose

Show the `PINS` backend with direct GPIO output.

## Hardware

- PIC18F452
- Output channels on PORTC bits 0..3
- One channel is used for the heartbeat pulse

## Build

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Expected result

Channel 0 pulses every half second and the startup pattern is emitted once.
