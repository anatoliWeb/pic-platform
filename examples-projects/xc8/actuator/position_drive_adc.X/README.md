# position_drive_adc.X

[Ukrainian version](./README.ua.md)

## Purpose

Closed-loop position drive example. A DC gear motor moves an arm to a requested angle using a
potentiometer as the position sensor (ADC backend of the `position_drive` library).

## What It Demonstrates

- `position_drive` init, `move_to_deg`, and non-blocking `process`
- ADC position sensor through the `read_raw` callback (`adc_read`)
- H-bridge direction control through the `motor` callback
- `tick` millisecond time source as the drive clock
- UART debug status output
- automatic move sequence 30 deg -> 120 deg -> loop

## Source Files

- `main.c`
- `config_bits.c`
- `../../../../core/delay.c`
- `../../../../drivers/analog/adc/adc.c`
- `../../../../drivers/gpio/gpio.c`
- `../../../../drivers/timers/tick/tick.c`
- `../../../../drivers/timers/timer1/timer1.c`
- `../../../../libraries/actuator/position_drive/position_drive.c`
- `../../../../libraries/system/uart_debug/uart_debug.c`

## Pin Mapping

- RA0/AN0: potentiometer wiper (position sensor)
- RD0: H-bridge IN1
- RD1: H-bridge IN2
- RC6: UART TX -> Proteus Virtual Terminal RXD (9600, 8N1)

## Drive Configuration

- raw range 0..1023 maps to 0..270 degrees
- tolerance 2 degrees
- move timeout 5000 ms
- stuck detection 1000 ms with 4 raw counts minimum movement

## Dynamic Behavior

- on boot the drive is initialized and the current potentiometer position is read
- the arm moves to 30 degrees, waits until the target is reached, then moves to 120 degrees
- every loop iteration `position_drive_process()` is called to keep the control non-blocking
- errors (timeout, stuck, sensor out of range) stop the motor and are reported over UART

## Notes

- `POSITION_DRIVE_ENABLE_UART_DEBUG=1` is set in the project so the library emits `PD:*`
  messages through the debug callback.
- the H-bridge mapping is fixed by wiring: FORWARD = IN1 high, REVERSE = IN2 high.
  Flip `direction_inverted` in the config if the wiring is reversed.

## Status

Ready for Proteus validation.
