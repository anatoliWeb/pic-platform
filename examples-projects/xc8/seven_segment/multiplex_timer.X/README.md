# multiplex_timer.X

[Ukrainian version](./README.ua.md)

## Purpose

Minimal timer-owned four-digit seven-segment diagnostic example for PIC18F452.
The goal is to verify that the library refresh path really runs from Timer2 without any manual refresh call in `main()`.

## What It Demonstrates

- `SEVEN_SEGMENT_REFRESH_TIMER`
- `SEVEN_SEGMENT_TIMER2`
- library-owned Timer2 setup
- `seven_segment_irq_handler()` from the global ISR
- timer-side refresh counter diagnostics
- fixed display content: `1234`

## Source Files

- `main.c`
- `config_bits.c`
- `../../../../core/delay.c`
- `../../../../drivers/gpio/gpio.c`
- `../../../../drivers/timers/timer2/timer2.c`
- `../../../../libraries/display/seven_segment/seven_segment.c`

## Refresh Ownership

- Refresh owner: library
- Selected timer: Timer2
- Requested period: `1000 us`
- Main-loop refresh call: not required in timer mode

## ISR Requirement

```c
void __interrupt() isr(void)
{
    seven_segment_irq_handler();
}
```

The application still enables `PEIE` and `GIE` after `seven_segment_init()` succeeds.

## Pin Mapping

- RD0..RD6: shared segments A..G
- RD7: shared decimal point
- RC0..RC3: digit enable lines
- RE0: init/API error indicator
- RE1: refresh counter activity indicator
- RE2: main loop heartbeat

## Expected Behavior

- The display shows constant `1234`.
- The display must keep refreshing without `seven_segment_process()` or `seven_segment_refresh()` in timer mode.
- `RE0` stays LOW during normal operation.
- `RE1` goes HIGH when `refresh_count` changes.
- `RE2` toggles in the main loop to show that the application is alive.

## Diagnostic Notes

- `APP_DIAGNOSTIC_MANUAL_FALLBACK = 0` keeps the example in pure timer-owned mode.
- `APP_DIAGNOSTIC_MANUAL_FALLBACK = 1` switches to manual refresh for comparison only.
- `seven_segment_process()` and `seven_segment_refresh()` are intentionally not used in timer mode.
- If the display is dark and `RE1 = 0`, investigate Timer2 backend, callback routing, and interrupt gates first.
- If the display is dark and `RE1 = 1`, investigate display GPIO wiring or polarity.

## Status

Ready for Proteus validation.
