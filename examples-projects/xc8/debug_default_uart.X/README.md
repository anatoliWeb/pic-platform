# debug_default_uart XC8 example

## Purpose

Demonstrate the default `UART` backend of `core/debug.h` without selecting a
backend explicitly.

## Hardware

- PIC18F452
- UART TX on RC6 / pin 25
- UART RX on RC7 / pin 26
- Virtual Terminal at 9600 baud, 8N1

## Build

```bash
mplab-make -f examples-projects/xc8/debug_default_uart.X/Makefile build
```

## Expected result

`BOOT` and a `tick=` counter appear on the terminal every second.
