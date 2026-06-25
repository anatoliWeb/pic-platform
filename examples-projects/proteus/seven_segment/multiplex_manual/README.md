# Proteus Notes: multiplex_manual

## Purpose

Placeholder documentation for validating fixed-point decimal formatting with manual multiplex refresh.

## Example Project

- `examples-projects/xc8/seven_segment/multiplex_manual.X`

## What To Check

- stable manual refresh at 100% brightness
- correct decimal-point placement for `12.34`, `7.5`, and `0.05`
- correct zero padding for `00.05`
- correct minus sign placement for `-12.3`
- RE1 heartbeat toggles when the scene changes
- RE0 stays low during normal operation

## Status

Ready for Proteus validation.
