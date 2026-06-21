# software_pwm Proteus Setup

## Purpose

Prepare the Proteus simulation notes for the initial XC8 `software_pwm.X` example.

## Components

- PIC18F452
- 10 MHz crystal with load capacitors
- 4 LEDs
- 4 resistors, 330R
- MCLR pull-up resistor, 10 k
- 5V supply and ground

## Wiring

- RB0 pin 33 -> LED0 -> 330R -> GND
- RB1 pin 34 -> LED1 -> 330R -> GND
- RB2 pin 35 -> LED2 -> 330R -> GND
- RB3 pin 36 -> LED3 -> 330R -> GND
- VDD pins 11 and 32 -> +5V
- VSS pins 12 and 31 -> GND
- MCLR pin 1 -> 10 k pull-up to +5V
- OSC1 pin 13 / OSC2 pin 14 -> 10 MHz crystal

## Expected Behavior

- Four LEDs show different average brightness levels.
- RB0 is the dimmest channel.
- RB3 is the brightest channel.
- All channels share the same PWM base frequency.

## Notes

- Create the Proteus `.pdsprj` manually when needed.
- Keep the future Proteus project under `examples-projects/proteus/software_pwm/`.
- Set the PIC Clock Frequency property in Proteus to `10MHz`.

## Status

Ready for Proteus validation.
