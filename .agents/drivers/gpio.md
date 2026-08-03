# gpio

## Purpose

Portable GPIO register helpers for TRIS/PORT access.

## Location

```text
drivers/gpio/gpio.h
drivers/gpio/gpio.c
XC8/drivers/gpio/gpio.c
C18/drivers/gpio/gpio.c
```

## Public API

| Function/type | Purpose | What to pass | Returns / output | Notes |
| --- | --- | --- | --- | --- |
| `gpio_init()` | optional project-level GPIO hook | nothing | none | may be empty |
| `gpio_set_output()` / `gpio_set_input()` | set TRIS direction | TRIS register pointer, bit | none | uses normalized bit writes |
| `gpio_write_high()` / `gpio_write_low()` / `gpio_toggle()` | drive output pin state | PORT register pointer, bit | none | pin must be configured as output |
| `gpio_read()` | read normalized pin state | PORT register pointer, bit | `0`/`1` | returns `0` on invalid args |

## Resource ownership

- No fixed peripheral ownership.
- Used by display, input, and actuator code.

## Examples

```text
examples-projects/xc8/input/button.X
examples-projects/xc8/seven_segment/*
examples-projects/xc8/actuator/position_drive_adc.X
```
