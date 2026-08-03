# debug_disabled XC8 example

## Purpose

Show that the debug API compiles with every `debug_*` call reduced to a no-op
when `DRV_DEBUG_ENABLE=0`.

## Selected defines

In `project_config.h`:

| Define | Value |
| --- | ---: |
| `DRV_DEBUG_ENABLE` | `0` |

With `DRV_DEBUG_ENABLE=0` the facade, all backends and every debug dependency
are compiled out. `debug.c` is not in the source list, so no UART, I2C, LCD or
GPIO backend code enters the image.

## Source isolation

This project compiles only `main.c`, `config_bits.c` and `core/delay.c`. No
`libraries/system/debug/*` source is compiled. The exact source list is in
`nbproject/configurations.xml`.

## Build

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Expected result

The project builds cleanly. The debug calls stay in the source but compile to
no-ops, so no debug hardware is required and no backend dependency is pulled
in.
