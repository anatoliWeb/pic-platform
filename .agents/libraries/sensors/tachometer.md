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

## API

| Function/type | Purpose | What to pass | Output | Notes |
| --- | --- | --- | --- | --- |
| `tachometer_t` | caller-owned state | nothing | struct | no heap |
| `tachometer_config_t` | timing and RPM thresholds | struct | config | validates in init |
| `tachometer_init()` | initialize state | tachometer, config | `drv_status_t` | rejects invalid config |
| `tachometer_set_expected_running()` | set armed-running mode | tachometer, flag, `now_us` | none | resets measurement on change |
| `tachometer_on_pulse()` | feed a pulse edge | tachometer, `now_us` | `uint8_t` | returns 1 when accepted |
| `tachometer_process()` | advance timeout/status | tachometer, `now_us` | none | non-blocking |
| `tachometer_get_rpm()` | read current RPM | tachometer | `uint16_t` | non-consuming |
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
- `signal_timeout_ms` moves the state to `NO_SIGNAL` when pulses stop.
- `minimum_pulse_interval_us` rejects noise before RPM is updated.

## ISR contract

- The module owns no ISR.
- A timer or external interrupt may call `tachometer_on_pulse()` with the pulse timestamp.
- The caller may call `tachometer_process()` from a main loop or timer task.

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
