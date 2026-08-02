# debug_disabled XC8 example

## Purpose

Show that the debug API compiles and runs when `DRV_DEBUG_ENABLE=0`.

## Hardware

- PIC18F452
- No debug hardware required

## Build

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Expected result

The project builds with all debug calls turned into no-ops.
