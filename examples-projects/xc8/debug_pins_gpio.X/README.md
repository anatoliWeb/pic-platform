# debug_pins_gpio XC8 example

## Purpose

Show the `PINS` backend with direct GPIO output.

## Hardware

- PIC18F452
- Output channels on PORTC bits 0..3
- One channel is used for the heartbeat pulse

## Build

```bash
mplab-make -f examples-projects/xc8/debug_pins_gpio.X/Makefile build
```

## Expected result

Channel 0 pulses every half second and the startup pattern is emitted once.
