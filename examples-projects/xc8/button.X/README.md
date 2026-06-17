# button.X

## Purpose

Stand-alone phase 1 button debounce example for PIC18F452.

## What it tests

- Active-low button sensing.
- Software debounce.
- LED toggle on a stable press.
- UART debug output.

## Proteus Components

- PIC18F452.
- Push button.
- 10 k pull-up resistor.
- LED.
- Optional Virtual Terminal.

## Pin Wiring

- RB0: button input, active-low.
- RB1: LED output.
- RC6/TX: optional Virtual Terminal RXD.

## Expected Behavior

- Released = HIGH.
- Pressed = LOW.
- A new stable press toggles the LED.
- Release does not toggle the LED again.

## Notes

- Debounce is polling-based.
- The button library supports press, release, click, double click, hold, and hold repeat events.
- Phase 2 menu navigation is planned separately and is not part of this example.

## Status

Ready for Proteus validation.