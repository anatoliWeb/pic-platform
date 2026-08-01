# position_drive_adc example

## Purpose

Closed-loop actuator example using ADC feedback and `position_drive`.

## Where to look

```text
examples-projects/xc8/actuator/position_drive_adc.X
examples-projects/hex/xc8/actuator/position_drive_adc.X.production.hex
examples-projects/proteus/actuator/position_drive_adc/README.md
```

## Integration notes

- ADC reads the potentiometer on `RA0/AN0`.
- `RD0` and `RD1` drive the H-bridge.
- `RD2` is optional PWM enable.
- `RC6/TX` connects to the Virtual Terminal.

## Files usually added

```text
main.c
config_bits.c
project_config.h
core/delay.c
drivers/analog/adc/adc.c
drivers/gpio/gpio.c
drivers/timers/tick/tick.c
drivers/timers/timer1/timer1.c
libraries/actuator/position_drive/position_drive.c
libraries/system/uart_debug/uart_debug.c
```
