[🇺🇦 Ukrainian version](./buzzer.ua.md)

# Buzzer Library

## Description

`buzzer` is a reusable audio notification library for embedded systems.
It supports both active buzzers (GPIO only) and passive buzzers (tone-capable backend).

## Public API

- `buzzer_init()`
- `buzzer_on()`
- `buzzer_off()`
- `buzzer_toggle()`
- `buzzer_beep()`
- `buzzer_update()`
- `buzzer_set_tone_backend()` (optional)
- `buzzer_play_tone()` (optional)
- `buzzer_play_pattern()` (optional)
- `buzzer_stop()` (optional)

## Playback Model (Non-Blocking)

The library uses an update-driven state machine:

- `buzzer_beep(duration_ms, now_ms)` schedules timed playback.
- `buzzer_play_pattern(...)` runs step-by-step pattern playback.
- `buzzer_update(now_ms)` advances timers and states.

No `delay_ms()` is used for playback timing.

## Active vs Passive Notes

- Active buzzer mode: direct GPIO on/off control.
- Passive buzzer mode: optional tone backend callbacks for frequency output.
- If tone callbacks are not set, passive mode falls back to GPIO level control.

## Pattern Engine

Pattern steps use `buzzer_pattern_step_t`:

- `duration_ms`
- `state` (on/off)
- `frequency_hz`

Supports:

- short beeps
- double beeps
- repeating sequences

## Example

`example.c` demonstrates:

- active buzzer short beep
- passive buzzer tone backend through PWM callbacks
- repeating non-blocking pattern with `tick_get()` integration

## Dependencies

- `core/*`
- `drivers/gpio`
- optional: `drivers/timers/pwm` (through callbacks in application/example)
- optional: `drivers/timers/tick` for timing source

## Embedded Constraints

- No dynamic memory
- Integer-only logic
- Lightweight RAM usage
