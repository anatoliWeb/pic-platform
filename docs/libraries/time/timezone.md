# Timezone Utilities Library

## Description

`timezone` provides compact UTC/local conversion utilities for embedded systems.
It is designed for fixed UTC offsets and optional simple DST handling.

This is not a full timezone database implementation.

## Public API

- `timezone_init()`
- `timezone_set_offset_minutes()`
- `timezone_set_dst()`
- `timezone_utc_to_local()`
- `timezone_local_to_utc()`
- `timezone_get_offset_minutes()` (optional)
- `timezone_is_dst_enabled()` (optional)

## Timezone Model

- Base offset in minutes (for example UTC+2 = `120`).
- Optional DST flag:
  - `0` = disabled
  - `1` = add one hour

Total offset:

- `offset_minutes * 60` seconds
- plus `3600` seconds if DST is enabled

## Datetime Model

Uses `rtc_datetime_t` from `rtc_framework`.
No duplicated datetime structures are introduced.

## Conversion Notes

- UTC -> local: `local = utc + offset`
- local -> UTC: `utc = local - offset`
- Internal conversion uses integer unix timestamp math.
- Leap-year/day/month logic is included in lightweight form.

## DST Notes

- Supports simple DST on/off only.
- No regional rules, timezone database, or historical transitions.
- Architecture is ready for future extension if rule-based DST is needed.

## Embedded Limitations

- No dynamic memory.
- No floating point.
- 32-bit unix conversion range limits apply.

## Dependencies

- `core/*`
- `libraries/time/rtc_framework`
