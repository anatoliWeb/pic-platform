# debug_display_parallel XC8 example

## Purpose

Show the `DISPLAY` backend with the LCD 2x16 parallel adapter.

## Hardware

- PIC18F452
- LCD 2x16 in 4-bit mode
- Data bus: PORTD
- RS/EN: RB0/RB1
- D4..D7: RD4..RD7

## Build

```bash
mplab-make -f examples-projects/xc8/debug_display_parallel.X/Makefile build
```

## Expected result

The LCD shows a boot message and a `tick=` counter.
