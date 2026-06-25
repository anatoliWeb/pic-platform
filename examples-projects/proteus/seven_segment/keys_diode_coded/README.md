# Proteus Notes: keys_diode_coded

## Purpose

Placeholder documentation for validating diode-coded shared-line keys with blink-mask selection and time/number switching.

## Example Project

- `examples-projects/xc8/seven_segment/keys_diode_coded.X`

## What To Check

- UP cycles through predefined blink masks
- DOWN switches between number mode and `HHMM` mode
- OK updates the active scene without clearing the display buffer model
- invalid or ambiguous masks do not change the visible content and do not create logical key events

## Status

Ready for Proteus validation.
