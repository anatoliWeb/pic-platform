# RTC Framework

## Description

`rtc_framework` is a unified abstraction layer for real-time clock implementations.
It provides one API for software RTC and hardware RTC backends (DS1302, DS3231, future devices).

## Public API

- `rtc_init()`
- `rtc_set_datetime()`
- `rtc_get_datetime()`
- `rtc_set_unix()`
- `rtc_get_unix()`
- `rtc_tick()` (optional software RTC tick)
- `rtc_is_valid()`

## Datetime Model

`rtc_datetime_t` contains:

- year, month, day
- hour, minute, second
- weekday

The framework validates date/time ranges and leap years.

## Backend Abstraction Model

Backend interface:

- `set_datetime(ctx, dt)`
- `get_datetime(ctx, dt)`

`rtc_t` can run in:

- hardware backend mode (driver callbacks provided)
- software RTC mode (no backend provided)

This keeps the API stable across different RTC chips.

## Unix Conversion Notes

Framework supports:

- datetime -> unix timestamp
- unix timestamp -> datetime

Implementation details:

- integer-only math
- leap year rules (400/100/4)
- epoch base: 1970-01-01

## Portability Goals

- same application code for DS1302, DS3231, software RTC
- no dynamic memory
- small RAM footprint
- embedded-friendly fixed logic

## Limitations

- Unix timestamp is 32-bit.
- No timezone/DST handling in framework layer.
- Alarm API is reserved for future extension.

## Dependencies

- `core/*`
