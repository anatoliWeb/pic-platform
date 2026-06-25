# Proteus Notes: multiplex_timer

## Purpose

Validation notes for the minimal timer-owned seven-segment diagnostic example.

## Example Project

- `examples-projects/xc8/seven_segment/multiplex_timer.X`

## What To Check

- The display shows constant `1234`.
- Timer2 refresh is owned by the library after `seven_segment_init()`.
- The global ISR only forwards control through `seven_segment_irq_handler()`.
- `seven_segment_process()` is not required in timer mode.
- `RE0` stays LOW during normal operation.
- `RE1` changes when the timer-owned refresh counter changes.
- `RE2` toggles to show that the main loop is still alive.

## Diagnostic Table

| Signal state | Meaning |
|---|---|
| `RE0 = 1` | init or API failure |
| `RE1 = 1` | timer refresh counter changes |
| `RE2 toggles` | main loop alive |
| display dark + `RE1 = 0` | Timer2 / ISR backend failure |
| display dark + `RE1 = 1` | GPIO refresh / polarity failure |

## Status

Ready for Proteus validation.
