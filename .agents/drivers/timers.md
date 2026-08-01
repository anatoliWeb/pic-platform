# timers

## Purpose

Timer driver map, including the millisecond tick timebase.

## Location

```text
drivers/timers/*
drivers/timers/tick/*
drivers/timers/timer0/*
drivers/timers/timer1/*
drivers/timers/timer2/*
drivers/timers/timer3/*
```

## What to open next

| Need | Open |
|---|---|
| Millisecond timebase | `.agents/core/timebase.md` |
| Timer ownership for seven-segment | `.agents/libraries/display/seven_segment.md` |
| Timeouts for actuator control | `.agents/libraries/actuator/position_drive.md` |

## Common ownership

- Timer1 -> `tick`
- Timer2 may be owned by `seven_segment`
- Timer0/3 are also potential `seven_segment` backends when enabled
