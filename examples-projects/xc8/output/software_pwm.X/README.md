# software_pwm.X

## Purpose

Initial software PWM example for PIC18F452 using one timer-driven PWM group.

## What it tests

- Software PWM for arbitrary GPIO outputs.
- One timer source driving one shared PWM group.
- Different duty cycle per channel with the same base PWM frequency.
- LED brightness control without using CCP hardware PWM.

## Proteus Components

- PIC18F452
- 10 MHz crystal and load capacitors
- 4 LEDs
- 4 resistors, 330R
- 10 k pull-up on MCLR

## Pin Wiring

- RB0: LED0 output, about 10% duty
- RB1: LED1 output, about 30% duty
- RB2: LED2 output, about 60% duty
- RB3: LED3 output, about 90% duty

## Expected Behavior

- Four LEDs show different brightness levels.
- RB0 is the dimmest channel.
- RB3 is the brightest channel.
- All channels share one PWM base frequency.

## Notes

- This example uses one software PWM group and `Timer2` as the tick source.
- It is intentionally separate from the hardware PWM example under `pwm.X`.
- `Timer0`, `Timer1`, and `Timer3` remain planned timer-source options for future validation.

## Status

Ready for Proteus validation.
