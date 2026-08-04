# AC Phase Control / Zero-Cross Output

## Description

`ac_phase_control` is a low-voltage, Proteus-first multi-channel phase-control group for PIC18F452.
It is separate from `software_pwm` because it does not generate a continuous duty-cycle waveform. Instead, one shared zero-cross event starts a new half-cycle and each enabled channel produces one short gate pulse at its own phase delay.

An optional per-channel relay bypass takes over the output at full power, so the triac stops switching and the relay drives the load through the entire half-cycle.

## Architecture

- One `ac_phase_control_group_t` owns shared timing and the channel array.
- Up to `AC_PHASE_CONTROL_MAX_CHANNELS` channels are supported.
- Each channel has its own gate pin, enabled state, power percent, and delay.
- Timer selection is passed through `ac_phase_control_init_group()`.
- `ac_phase_control_attach_channel()` expects a gate **bit mask** such as `(1U << 0U)` for `RD0`.
- `ac_phase_control_attach_channel_relay()` expects a relay **bit mask** such as `(1U << 4U)` for `RD4`.
- Timer-driver selection and reload logic stay inside the library.
- The application only calls the public `ac_phase_control_irq_handler()` from its ISR.
- No dynamic allocation is used.
- Integer-only timing is used.
- The group is armed only after a zero-cross event and becomes inactive again when the half-cycle ends.
- Relay transitions use a non-blocking state machine advanced from `ac_phase_control_process()`, so no blocking delay is required.

## Relay Mode

- A channel may run in phase mode only, or in phase mode with a relay bypass attached.
- The relay engages when `power_percent >= relay_on_threshold_percent` (default `98%`).
- The relay disengages when `power_percent <= relay_off_threshold_percent` (default `96%`).
- The difference between the thresholds provides hysteresis and prevents relay chatter around the transition point.
- `relay_break_before_make_ms` (default `50 ms`) forces the triac gate silent before the relay engages and holds the relay off after the request drops.
- `relay_min_on_ms` (default `200 ms`) and `relay_min_off_ms` (default `200 ms`) enforce minimum dwell times.
- While a relay is engaging, engaged, or releasing, the channel's gate pulses are suppressed so the triac and the relay never drive the same load simultaneously.
- At `100%` the gate stays silent and the relay drives the full half-cycle.
- `ac_phase_control_is_channel_in_relay_mode()` reports whether a channel is currently holding its relay on.

## Zero-Cross Timeout

- When `zero_cross_timeout_ms` is non-zero, `ac_phase_control_process()` compares the elapsed time since the last zero-cross against the timeout.
- If the timeout expires, the group enters `AC_PHASE_STATUS_ZERO_CROSS_LOST`, clears all gate pulses, releases all relays, and applies an all-off state.
- The group recovers automatically after `AC_PHASE_CONTROL_ZERO_CROSS_RECOVERY_EVENTS` (default `2`) fresh zero-cross events.
- `ac_phase_control_get_status()` returns the group status and `ac_phase_control_is_zero_cross_alive()` reports whether the zero-cross stream is healthy.

## Public API

- `ac_phase_control_init_group()`
- `ac_phase_control_attach_channel()`
- `ac_phase_control_attach_channel_relay()`
- `ac_phase_control_detach_channel()`
- `ac_phase_control_set_power_percent()`
- `ac_phase_control_set_delay_us()`
- `ac_phase_control_enable_channel()`
- `ac_phase_control_get_power_percent()`
- `ac_phase_control_is_channel_enabled()`
- `ac_phase_control_is_channel_in_relay_mode()`
- `ac_phase_control_on_zero_cross()`
- `ac_phase_control_update_us()`
- `ac_phase_control_process()`
- `ac_phase_control_all_off()`
- `ac_phase_control_stop_group()`
- `ac_phase_control_irq_handler()`
- `ac_phase_control_get_tick_ms()`
- `ac_phase_control_is_any_channel_active()`
- `ac_phase_control_get_status()`
- `ac_phase_control_is_zero_cross_alive()`

## Timer Selection

- Library timer enum:
  - `AC_PHASE_CONTROL_TIMER2`
- Default example alias:
  - `AC_PHASE_CONTROL_SELECTED_TIMER`
- Default source:
  - `Timer2`
- In this stage, only the validated `Timer2` path is implemented.
- The application must not include timer driver headers directly.
- The example keeps `Timer2` as the validated default and uses the existing timer driver API only.

## Timing Model

- Default AC simulation target is `50 Hz`.
- One full cycle is `20 ms`.
- One half-cycle is `10 ms`.
- `ac_phase_control_on_zero_cross()` starts a fresh half-cycle.
- `ac_phase_control_update_us()` advances elapsed time without blocking.
- Each enabled channel may generate at most one short gate pulse per half-cycle.
- Larger elapsed values are handled with wider temporary integers to avoid overflow.
- The gate output returns LOW when the pulse ends or when the half-cycle expires.
- A `100 Hz` zero-cross pulse source matches the default `50 Hz` target: one pulse per half-cycle.

## Examples

- `examples-projects/xc8/output/ac_phase_control.X`: four phase-only channels with a slow fade on channel `0`.
- `examples-projects/xc8/output/multi_channel_relay.X`: four channels where channels `0..2` also carry relay bypass outputs; channel `0` walks `50% -> 97% -> 98% -> 100% -> 50%` to exercise the relay transition, and `RB1` provides emergency all-off.

## Validation Status

- `Timer2`: implemented and used as the default example source.
- `Timer0/1/3` are reserved for future work and are not part of this stage.
- Multi-channel Proteus validation: pending.
- Relay-bypass Proteus validation: pending.
- Slow fade Proteus validation: pending.

## Proteus Notes

- Use one fake zero-cross pulse source on `RB0 / INT0`.
- Probe `RD0..RD3` with an oscilloscope or logic analyzer.
- Optional diagnostic pins:
  - `RC0`: zero-cross toggle
  - `RC1`: timer callback toggle
  - `RC2`: gate-active indicator
- All channels must synchronize to the same zero-cross edge.
- Lower configured power means a later gate pulse.
- `0%` produces no pulse.
- `100%` produces a pulse near `min_delay_us`.
- No output should stay HIGH.
- Real mains and `220V AC` are out of scope for this module.

## Status

Ready for Proteus validation.
