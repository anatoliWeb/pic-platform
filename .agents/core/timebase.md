# timebase

## Purpose

Shared clock and timeout guidance for the platform.

## Location

```text
drivers/timers/tick/
drivers/timers/timer1/
core/device.h
```

## Use when

- a module needs `tick_get()` for timeouts, debounce, or non-blocking scheduling.

## Do not use when

- Timer1 is already owned by another component in the same project.

## Public API

| Function/type | Purpose | What to pass | Returns / output | Notes |
| --- | --- | --- | --- | --- |
| `tick_init()` | initialize millisecond tick | nothing | configures Timer1 | owns Timer1 in this repo |
| `tick_get()` | current milliseconds | nothing | `uint32_t` time | used by `button` and `position_drive` |
| `tick_delay(ms)` | blocking delay | milliseconds | none | use sparingly |

## Required files

```text
drivers/timers/tick/tick.h
drivers/timers/tick/tick.c
drivers/timers/timer1/timer1.h
drivers/timers/timer1/timer1.c
```

## Dependencies

```text
core/device.h
```

## Resource ownership

- Timer1 is owned by `tick`.
- `seven_segment` timer mode must not collide with that ownership.
