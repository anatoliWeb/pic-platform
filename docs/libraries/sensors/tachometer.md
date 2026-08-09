# tachometer

Reusable pulse-to-RPM helper with startup grace, noise rejection, timeout, ISR-safe critical sections, and status.

## Compile-time profiles

### FULL (default)

Full tachometer behavior with RPM calculation, minimum RPM check, and all diagnostic statuses.

### LIGHTWEIGHT

Define `TACHOMETER_LIGHTWEIGHT=1` before including the header to enable the lightweight profile:

- Disables the expensive 64-bit RPM division; `tachometer_get_rpm()` always returns 0
- `minimum_rpm` is still enforced: the module computes a maximum pulse-interval threshold once in `tachometer_init()` and raises `TOO_SLOW` when the accepted pulse interval exceeds it. The check is bit-exact with the FULL profile, except that a zero interval is also reported `TOO_SLOW` (mirrors FULL with the noise filter disabled)
- All pulse filtering, startup grace, timeout, and presence detection remain fully functional
- Saves 524 B ROM on PIC18F452 (measured with XC8 3.10) versus the FULL profile; RAM usage is identical (103 B)

Usage:
```c
#define TACHOMETER_LIGHTWEIGHT 1
#include "libraries/sensors/tachometer/tachometer.h"
```

## API

| Item | Notes |
| --- | --- |
| `tachometer_init()` | validates config and starts in `STOPPED` or `CONFIG_ERROR` |
| `tachometer_set_expected_running()` | explicit running expectation, caller passes `now_us` |
| `tachometer_on_pulse()` | accepts a pulse timestamp and updates RPM; ISR-safe |
| `tachometer_process()` | advances timeout state without blocking |
| `tachometer_get_rpm()` | returns snapshot of current RPM; macro returns 0 in LIGHTWEIGHT |
| `tachometer_get_status()` | returns snapshot of current status |
| `tachometer_get_pulse_count()` | returns cumulative accepted pulse count |
| `tachometer_reset()` | clears runtime state, keeps config |

## ISR contract

- `tachometer_on_pulse()` is safe to call from timer or external interrupt context. It uses `DRV_INT_SAVE_AND_DISABLE` / `DRV_INT_RESTORE` from `core/compiler.h` to protect shared fields. These macros save the previous GIE state and restore it exactly, so they are safe from ISR context (where GIE is already 0) and from the main loop (where GIE is 1).
- `tachometer_process()` takes an atomic snapshot of ISR-written fields (`last_pulse_us`, `session_state`, `rpm`, `expected_running`, `expected_running_since_us`) under a short critical section. All timeout and status calculations use the snapshot. Before committing any state change (rearm, status update), it re-verifies that `last_pulse_us` has not changed — if it changed, a newer pulse arrived and the stale result is discarded.
- Getters (`get_rpm`, `get_status`, `get_pulse_count`) return consistent single-field snapshots protected by short critical sections.
- `init`, `set_expected_running`, and `reset` are main-loop only.

## Behavior

- RPM is computed as `60000000 / (pulse_interval_us * pulses_per_revolution)`.
- In LIGHTWEIGHT, `minimum_rpm` is enforced by a precomputed maximum pulse-interval threshold `60000000 / (minimum_rpm * pulses_per_revolution)`, compared in `tachometer_on_pulse()`.
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
