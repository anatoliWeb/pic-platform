# debug_disabled XC8 example

## Purpose

Show that the debug API compiles and runs when `DRV_DEBUG_ENABLE=0`.

## Hardware

- PIC18F452
- No debug hardware required

## Build

```bash
mplab-make -f examples-projects/xc8/debug_disabled.X/Makefile build
```

## Expected result

The project builds with all debug calls turned into no-ops.
