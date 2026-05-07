# Relay Control Library

## Description

`relay` is a lightweight reusable relay control library for embedded automation tasks.
It supports both simple on/off control and non-blocking timed operations.

## Public API

- `relay_init()`
- `relay_on()`
- `relay_off()`
- `relay_toggle()`
- `relay_set_state()`
- `relay_get_state()`
- `relay_set_inverted()` (optional)
- `relay_pulse()` (optional)
- `relay_delayed_off()` (optional)
- `relay_update()` (optional timing handler)

## Inversion Logic (Active HIGH vs Active LOW)

- `inverted = 0`: active HIGH modules
- `inverted = 1`: active LOW modules

Library keeps logical state (`on/off`) and maps it to physical GPIO level.

## Relay Timing Model

No blocking delays are used.
Timed behavior is implemented with periodic update:

- `relay_pulse(duration_ms, now_ms)`
- `relay_delayed_off(delay_ms, now_ms)`
- `relay_update(now_ms)` checks deadlines and applies OFF when time is reached

This works well with scheduler/tick based application loops.

## Safety Notes

- `safe_start_off` option in `relay_init()` allows fail-safe startup.
- Architecture is ready for watchdog/fail-safe extensions at application level.
- Use external hardware protection (flyback diodes/opto-isolation) as required by relay board.

## Embedded Constraints

- No dynamic memory
- Integer-only logic
- Small RAM footprint

## Dependencies

- `core/*`
- `drivers/gpio`
- optional integration with scheduler/tick in application layer
