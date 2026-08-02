# debug_display_i2c XC8 example

## Purpose

Show the `DISPLAY` backend with the LCD 2x16 I2C adapter.

## Hardware

- PIC18F452
- LCD 2x16 with PCF8574 backpack at `0x27`
- I2C pins on RC3/SCL and RC4/SDA

## Build

```bash
mplab-make -f examples-projects/xc8/debug_display_i2c.X/Makefile build
```

## Expected result

The display is cleared and refreshed with a boot line and a `tick=` counter.
