# adc_buttons.X

## Purpose

ADC ladder button example for a single analog input on PIC18F452.

## What it tests

- ADC range matching.
- Stable button state detection.
- LED toggling on a new stable button.
- Raw ADC value logging over UART.

## Proteus Components

- PIC18F452.
- Resistor ladder connected to AN0.
- Four buttons in the ladder.
- Four LEDs on RB0..RB3.
- Virtual Terminal.
- Timer1 tick support.

## Pin Wiring

- RA0 / AN0: ladder input.
- RB0..RB3: LED outputs.
- RC6/TX: UART debug output.
- Timer1 interrupt: required for tick-based debounce.

## Expected Behavior

- A stable ADC range maps to one logical button ID.
- Pressing a new button toggles the matching LED.
- Holding the same button does not keep retoggling it.
- Raw ADC changes are printed to the terminal.

## Notes

- The example uses tuned ADC ranges from observed Proteus values.
- Adjust the table if the resistor ladder changes.
- This example uses stable button changes only; click and hold semantics are not part of the sample.
- Menu logic is intentionally outside scope for Phase 1.

## Status

Verified in Proteus.

Manual Proteus validation completed successfully.