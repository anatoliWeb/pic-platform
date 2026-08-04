# multi_channel_relay.X

## Purpose

Multi-channel phase-control example for PIC18F452 that demonstrates the optional per-channel relay bypass: a triac phase-control output with an independent relay that takes over at full power.

## What it tests

- Shared zero-cross synchronization on `RB0 / INT0`
- Four independent phase-control channels
- Relay bypass on channels 0, 1, and 2 (channel 3 is phase-only)
- Hysteresis between relay ON threshold (`98%`) and OFF threshold (`96%`)
- Break-before-make, minimum relay ON/OFF times
- `100%` request keeps the relay engaged and the triac gate silent
- Zero-cross timeout fail-safe: `ac_phase_control_process()` enters `ZERO_CROSS_LOST` and switches all outputs off if the `RB0` pulses stop for `zero_cross_timeout_ms`
- Emergency all-off via `RB1` (active low)

## Timer Source

- The timer source is selected in [project_config.h](./project_config.h).
- Default alias: `AC_PHASE_CONTROL_SELECTED_TIMER`
- Default value: `AC_PHASE_CONTROL_TIMER2`
- Available timer enum values:
  - `AC_PHASE_CONTROL_TIMER2`

## Pin Wiring

- `RB0 / INT0`: fake zero-cross pulse source at `100 Hz`
- `RB1`: emergency all-off input (active low)
- `RD0`: channel 0 triac gate
- `RD1`: channel 1 triac gate
- `RD2`: channel 2 triac gate
- `RD3`: channel 3 triac gate
- `RD4`: channel 0 relay bypass
- `RD5`: channel 1 relay bypass
- `RD6`: channel 2 relay bypass
- Channel attachment uses bit masks, for example `RD0 -> 1 << 0`, not raw bit numbers.
- The group arms only after zero-cross and becomes inactive again when the half-cycle ends.

## Expected Behavior

- All outputs synchronize to the same zero-cross pulse on `RB0`.
- Each channel emits no more than one short gate pulse per half-cycle.
- Channel 0 walks `50% -> 97% -> 98% -> 100% -> 50%`: relay engages at `98%` (with hysteresis) and disengages at `96%`.
- Channels 1 and 2 stay at fixed `50%` and `100%`.
- Channel 3 stays at fixed `30%` (no relay).
- At `100%` the triac gate goes silent and the relay drives the full half-cycle.
- `0%` produces no pulse.
- No output should stay HIGH.
- No pulse should appear without zero-cross.

## Notes

- This is phase-control simulation, not ordinary PWM.
- The waveform is the primary validation target.
- `Timer2` is the validated default source in this stage.
- Real mains and `220V AC` remain out of scope.
- Optional diagnostic pins:
  - `RC0`: zero-cross toggle
  - `RC1`: timer callback toggle
  - `RC2`: gate-active indicator

## Status

Ready for Proteus validation.
