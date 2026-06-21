# AC Phase Control / Zero-Cross Output

## Description

`ac_phase_control` is a low-voltage, Proteus-first multi-channel phase-control group for PIC18F452.
It is separate from `software_pwm` because it does not generate a continuous duty-cycle waveform. Instead, one shared zero-cross event starts a new half-cycle and each enabled channel produces one short gate pulse at its own phase delay.

## Architecture

- One `ac_phase_control_group_t` owns shared timing and the channel array.
- Up to `AC_PHASE_CONTROL_MAX_CHANNELS` channels are supported.
- Each channel has its own gate pin, enabled state, power percent, and delay.
- Timer selection is passed through `ac_phase_control_init_group()`.
- `ac_phase_control_attach_channel()` expects a gate **bit mask** such as `(1U << 0U)` for `RD0`.
- Timer-driver selection and reload logic stay inside the library.
- The application only calls the public `ac_phase_control_irq_handler()` from its ISR.
- No dynamic allocation is used.
- Integer-only timing is used.
- The group is armed only after a zero-cross event and becomes inactive again when the half-cycle ends.

## Public API

- `ac_phase_control_init_group()`
- `ac_phase_control_attach_channel()`
- `ac_phase_control_detach_channel()`
- `ac_phase_control_set_power_percent()`
- `ac_phase_control_set_delay_us()`
- `ac_phase_control_enable_channel()`
- `ac_phase_control_get_power_percent()`
- `ac_phase_control_is_channel_enabled()`
- `ac_phase_control_on_zero_cross()`
- `ac_phase_control_update_us()`
- `ac_phase_control_stop_group()`
- `ac_phase_control_irq_handler()`
- `ac_phase_control_get_tick_ms()`
- `ac_phase_control_is_any_channel_active()`

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

## Fade Behavior

- The XC8 example uses a slow non-blocking fade on channel `0`.
- The fade is driven from a millisecond helper returned by the library.
- `RD0` moves `0% -> 100% -> 0%`.
- `RD1`, `RD2`, and `RD3` remain fixed at `25%`, `50%`, and `75%`.

## Validation Status

- `Timer2`: implemented and used as the default example source.
- `Timer0/1/3` are reserved for future work and are not part of this stage.
- Multi-channel Proteus validation: pending.
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
