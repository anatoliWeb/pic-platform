# Software RTC Library

## Description

`software_rtc` is a lightweight RTC backend that works without external RTC hardware.
It is designed for simulation, testing, Proteus setups, and fallback runtime clock use.

## Public API

- `software_rtc_init()`
- `software_rtc_tick_ms()`
- `software_rtc_tick_seconds()`
- `software_rtc_set_datetime()`
- `software_rtc_get_datetime()`
- `software_rtc_sync()` (optional)
- `software_rtc_is_running()` (optional)
- `software_rtc_get_driver()` (backend adapter for `rtc_framework`)

## Tick Model

- `software_rtc_tick_ms()` accumulates milliseconds.
- When accumulated time reaches 1000 ms, seconds are advanced.
- `software_rtc_tick_seconds()` can directly advance whole seconds.
- Date/time rollover handles:
  - seconds/minutes/hours
  - day/month/year
  - leap years
  - weekday progression

No delay-based timing is used.

## Backend Integration

The library exposes `software_rtc_get_driver()` returning `rtc_driver_t`.
This allows direct integration with `rtc_framework`:

- `rtc_init(&rtc, software_rtc_get_driver(), &sw_ctx);`

Datetime/unix conversion remains centralized in `rtc_framework`.

## Architecture Notes

- No dynamic memory.
- Integer-only logic.
- Small state:
  - current datetime
  - millisecond accumulator
  - running flag

## Drift and Synchronization

- Software RTC accuracy depends on source tick quality.
- If scheduler/timer jitter exists, drift will accumulate.
- Use `software_rtc_sync()` to re-align with trusted time source when needed.

## Simulator / Proteus Usage

- Useful when no RTC chip is present.
- Can be driven by periodic callback from timer ISR or task scheduler.
- Good for deterministic test scenarios.

## Dependencies

- `core/*`
- `libraries/time/rtc_framework`
