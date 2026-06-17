# encoder.X

## Purpose

Stand-alone phase 1 quadrature encoder example for PIC18F452.

## What it tests

- Direction detection.
- Delta output.
- Absolute position counter.
- Optional encoder button events over UART.

## Proteus Components

- PIC18F452.
- Rotary encoder with push button.
- Optional pull-up resistors or Proteus PORTB pull-ups.
- Virtual Terminal.

## Pin Wiring

- RB0: encoder A.
- RB1: encoder B.
- RB2: encoder SW.
- Optional pull-ups on A, B, and SW.
- RC6/TX: UART debug output.

## Expected Behavior

- Rotate left or right to change delta and position.
- Push the encoder switch to print press, release, click, double click, hold, and hold repeat events.
- No menu logic is included in this phase.

## Notes

- Polling every 1-5 ms is recommended.
- The encoder button uses the existing `button_t` helper.
- Phase 2 menu navigation is planned separately and is not part of this example.

## Status

Ready for Proteus validation.