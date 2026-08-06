# tachometer

Reusable pulse-to-RPM helper with startup grace, noise rejection, timeout, ISR-safe critical sections, and status.

## API

| Item | Notes |
| --- | --- |
| `tachometer_init()` | validates config and starts in `STOPPED` or `CONFIG_ERROR` |
| `tachometer_set_expected_running()` | explicit running expectation, caller passes `now_us` |
| `tachometer_on_pulse()` | accepts a pulse timestamp and updates RPM; ISR-safe |
| `tachometer_process()` | advances timeout state without blocking |
| `tachometer_get_rpm()` | returns snapshot of current RPM |
| `tachometer_get_status()` | returns snapshot of current status |
| `tachometer_get_pulse_count()` | returns cumulative accepted pulse count |
| `tachometer_reset()` | clears runtime state, keeps config |

## ISR contract

- `tachometer_on_pulse()` is safe to call from timer or external interrupt context. It uses `DRV_INT_SAVE_AND_DISABLE` / `DRV_INT_RESTORE` from `core/compiler.h` to protect shared fields. These macros save the previous GIE state and restore it exactly, so they are safe from ISR context (where GIE is already 0) and from the main loop (where GIE is 1).
- `tachometer_process()` must be called from the main loop. It does NOT use critical sections; it reads ISR-written fields that may change at any time.
- Getters (`get_rpm`, `get_status`, `get_pulse_count`) return consistent single-field snapshots protected by short critical sections.
- `init`, `set_expected_running`, and `reset` are main-loop only.

## Behavior

- RPM is computed as `60000000 / (pulse_interval_us * pulses_per_revolution)`.
- `timestamp_us == 0` is valid.
- Time comparisons are wrap-safe.
- Two pulses are needed to compute an RPM; the first pulse of a session only re-arms the measurement state.
- After `signal_timeout_ms` without a pulse the session re-arms: RPM returns to 0, the stale pulse timestamp is discarded, and a fresh session starts on the next pulse. The cumulative `pulse_count` is not reset by a timeout.
- `pulse_count` counts accepted pulses since init, reset, or an expected-running change; it survives timeouts.
- `session_state` is a named measurement-session phase (`TACHOMETER_SESSION_UNARMED` / `FIRST_PULSE` / `ACTIVE`), not a cumulative counter.
- Long pulse streams saturate `session_state` at `ACTIVE`; it stays in that phase after the second accepted pulse. The cumulative `pulse_count` keeps growing.

## Examples

- `examples-projects/xc8/sensors/tachometer.X`

## Tests

- `scripts/tests/test_tachometer.py`
