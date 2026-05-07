# Cron Scheduler Library

## Description

`cron_scheduler` is a compact embedded cron-style scheduler integrated with `rtc_framework`.
It is intentionally minimal and optimized for MCU usage.

This is not a full Linux cron parser.

## Public API

- `cron_scheduler_init()`
- `cron_scheduler_add()`
- `cron_scheduler_remove()`
- `cron_scheduler_update()`
- `cron_scheduler_enable()` (optional)
- `cron_scheduler_disable()` (optional)

## Scheduler Model

Each task rule includes:

- minute
- hour
- day
- month
- weekday

Each field supports:

- exact value
- wildcard `CRON_ANY` (`*` equivalent)

Tasks are stored in fixed pool:

- `CRON_MAX_TASKS`
- no dynamic allocation

## Callback Model

Task callback type:

`typedef void (*cron_callback_t)(void* ctx);`

Each task stores:

- callback function
- callback context pointer
- enabled/used flags

## Update Model

`cron_scheduler_update()`:

1. reads current datetime from `rtc_framework`
2. checks all enabled tasks
3. runs matching callbacks
4. stores last run timestamp to prevent duplicate execution in same minute

This function should be called periodically from main loop/scheduler task.

## Embedded Limitations

- No text cron parser
- No dynamic task creation
- No timezone/DST logic
- Minute-level granularity for trigger matching

## Timing Recommendations

- Call `cron_scheduler_update()` at least once per second.
- More frequent calls are safe due to duplicate protection.

## Dependencies

- `core/*`
- `libraries/time/rtc_framework`
