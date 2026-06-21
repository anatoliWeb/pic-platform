# Proteus: ac_phase_control

## Purpose

Manual Proteus validation target for the multi-channel low-voltage phase-control example.

## Recommended Connections

- `RB0`: fake zero-cross pulse source at `100 Hz`
- `RD0`: oscilloscope channel A, slow fade channel
- `RD1`: oscilloscope or logic analyzer, fixed `25%`
- `RD2`: logic analyzer, fixed `50%`
- `RD3`: logic analyzer, fixed `75%`

## Selected Timer Source

- Default example source: `Timer2`
- Timer selection is controlled in `examples-projects/xc8/ac_phase_control.X/project_config.h`

## What to Check

- All outputs synchronize to the same `RB0` zero-cross pulse.
- Each channel generates no more than one short gate pulse per half-cycle.
- `RD1`, `RD2`, and `RD3` produce stable but different delays.
- `RD0` slowly moves its pulse timing as the fade changes from `0%` to `100%` and back.
- `0%` produces no pulse.
- `100%` produces a pulse near `min_delay_us`.
- No output stays HIGH.
- No pulse appears without zero-cross.

## Notes

- This is phase-control simulation, not regular PWM.
- Later gate pulses correspond to lower configured power.
- Perceived lamp brightness is not linearly proportional to the simple delay value.
- Real mains and `220V AC` remain out of scope.

## Status

Ready for Proteus validation.
