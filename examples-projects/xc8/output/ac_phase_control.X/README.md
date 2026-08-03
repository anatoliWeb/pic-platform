# ac_phase_control.X

## Purpose

Multi-channel low-voltage phase-control example for PIC18F452 using one shared zero-cross input and four independent gate outputs.

## What it tests

- Shared zero-cross synchronization on `RB0 / INT0`
- Four independent phase-control channels
- Different phase delays for different configured power levels
- One non-blocking slow fade on `RD0`
- Library-owned timer selection and timer interrupt dispatch

## Timer Source

- The timer source is selected in [project_config.h](./project_config.h).
- Default alias: `AC_PHASE_CONTROL_SELECTED_TIMER`
- Default value: `AC_PHASE_CONTROL_TIMER2`
- Available timer enum values:
  - `AC_PHASE_CONTROL_TIMER2`

## Pin Wiring

- `RB0 / INT0`: fake zero-cross pulse source at `100 Hz`
- `RD0`: phase-control channel 0, slow fade `0% -> 100% -> 0%`
- `RD1`: phase-control channel 1, fixed `25%`
- `RD2`: phase-control channel 2, fixed `50%`
- `RD3`: phase-control channel 3, fixed `75%`
- Channel attachment uses bit masks, for example `RD0 -> 1 << 0`, not raw bit numbers.
- The group arms only after zero-cross and becomes inactive again when the half-cycle ends.

## Expected Behavior

- All outputs synchronize to the same zero-cross pulse on `RB0`.
- Each channel emits no more than one short gate pulse per half-cycle.
- `RD1`, `RD2`, and `RD3` keep stable but different delays.
- `RD0` slowly moves its pulse timing as the fade changes from `0%` to `100%` and back.
- `0%` produces no pulse.
- `100%` produces a pulse near `min_delay_us`.
- No output should stay HIGH.
- No pulse should appear without zero-cross.

## Notes

- This is phase-control simulation, not ordinary PWM.
- The waveform is the primary validation target.
- `Timer2` is the validated default source in this stage.
- `Timer0`, `Timer1`, and `Timer3` are reserved for future work and are not part of this stage.
- Real mains and `220V AC` remain out of scope.
- Optional diagnostic pins:
  - `RC0`: zero-cross toggle
  - `RC1`: timer callback toggle
  - `RC2`: gate-active indicator

## Status

Ready for Proteus validation.
