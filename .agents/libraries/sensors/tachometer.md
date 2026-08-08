# tachometer

## Purpose

Reusable pulse-to-RPM tachometer with noise rejection, startup grace, timeout, and diagnostic status.

## Location

```text
libraries/sensors/tachometer/tachometer.h
libraries/sensors/tachometer/tachometer.c
libraries/sensors/tachometer/example.c
docs/libraries/sensors/tachometer.md
docs/libraries/sensors/tachometer.ua.md
examples-projects/xc8/sensors/tachometer.X
scripts/tests/test_tachometer.py
```

## Use when

- you need reusable pulse interval to RPM conversion;
- you need minimum pulse interval filtering;
- you need startup grace, expected-running state, and signal loss status.

## Do not use when

- you need fan, heater, or PWM policy;
- you want board pin binding in the library;
- you only need a raw interrupt flag.

## Compile-time profiles

### FULL (default)

Full tachometer behavior with RPM calculation, minimum RPM check, and all diagnostic statuses.

### LIGHTWEIGHT

Define `TACHOMETER_LIGHTWEIGHT=1` before including the header to enable the lightweight profile:

- Disables expensive 64-bit division RPM calculation
- `tachometer_get_rpm()` always returns 0
- TOO_SLOW status is never set
- All pulse filtering, startup grace, timeout, and presence detection remain fully functional
- Saves ~1236 B ROM on PIC18F452 (measured with XC8 3.10)

Usage:
```c
#define TACHOMETER_LIGHTWEIGHT 1
#include "libraries/sensors/tachometer/tachometer.h"
```

## API

| Function/type | Purpose | What to pass | Output | Notes |
| --- | --- | --- | --- | --- |
| `tachometer_t` | caller-owned state | nothing | struct | no heap |
| `tachometer_config_t` | timing and RPM thresholds | struct | config | validates in init |
| `tachometer_init()` | initialize state | tachometer, config | `drv_status_t` | rejects invalid config |
| `tachometer_set_expected_running()` | set armed-running mode | tachometer, flag, `now_us` | none | resets measurement on change |
| `tachometer_on_pulse()` | feed a pulse edge | tachometer, `now_us` | `uint8_t` | returns 1 when accepted |
| `tachometer_process()` | advance timeout/status | tachometer, `now_us` | none | non-blocking |
| `tachometer_get_rpm()` | read current RPM | tachometer | `uint16_t` | non-consuming; macro returns 0 in LIGHTWEIGHT |
| `tachometer_get_status()` | read status | tachometer | `tachometer_status_t` | non-consuming |
| `tachometer_get_pulse_count()` | read accepted pulse count | tachometer | `uint32_t` | non-consuming |
| `tachometer_reset()` | clear runtime state | tachometer | none | keeps config |

## Dependencies

```text
core/compiler.h
core/types.h
```

## Runtime/time model

- Caller passes monotonic microseconds.
- `startup_grace_ms` suppresses `TOO_SLOW` during startup.
- Two pulses are needed to compute RPM; the first pulse of a session only re-arms the measurement state.
- `signal_timeout_ms` moves the state to `NO_SIGNAL` and re-arms the session (RPM 0, stale timestamp discarded) when pulses stop. The cumulative `pulse_count` survives timeouts.
- `session_state` is a named measurement-session phase (`TACHOMETER_SESSION_UNARMED` / `FIRST_PULSE` / `ACTIVE`), saturated at `ACTIVE`, and is not a cumulative counter.
- `pulse_count` remains the cumulative accepted pulse counter.
- `minimum_pulse_interval_us` rejects noise before RPM is updated.

## ISR contract

- The module owns no ISR.
- A timer or external interrupt may call `tachometer_on_pulse()` with the pulse timestamp. This function is ISR-safe and uses `DRV_INT_SAVE_AND_DISABLE` / `DRV_INT_RESTORE` from `core/compiler.h` to protect shared fields.
- `tachometer_process()` must be called from the main loop. It takes an atomic snapshot of ISR-written fields under a short critical section, computes timeout/status using the snapshot, and re-verifies `last_pulse_us` before committing any state change to prevent stale results from overwriting fresh pulse data.
- Getters (`get_rpm`, `get_status`, `get_pulse_count`) return consistent single-field snapshots protected by short critical sections and are main-loop only.
- `init`, `set_expected_running`, and `reset` are main-loop only; do not call from ISR.
- Multiple instances are fully independent; no shared global state.

## Resource ownership

- No pin ownership.
- No timer ownership.
- No fan, relay, heater, or PWM ownership.

## Examples

```text
examples-projects/xc8/sensors/tachometer.X
```

## Tests

```text
scripts/tests/test_tachometer.py
scripts/tests/test_examples_hierarchy.py
scripts/tests/test_check_agents_catalog.py
```

## Docs

```text
docs/libraries/sensors/tachometer.md
docs/libraries/sensors/tachometer.ua.md
```

## Limitations

- Requires monotonic microsecond timestamps.
- RPM is derived from the last accepted pulse interval.

## AI decision rule

Use this module for reusable tachometer logic. Do not add application policy into it.
