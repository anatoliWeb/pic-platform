# tachometer

Reusable pulse-to-RPM helper with startup grace, noise rejection, timeout, and status.

## API

| Item | Notes |
| --- | --- |
| `tachometer_init()` | validates config and starts in `STOPPED` or `CONFIG_ERROR` |
| `tachometer_set_expected_running()` | explicit running expectation, caller passes `now_us` |
| `tachometer_on_pulse()` | accepts a pulse timestamp and updates RPM |
| `tachometer_process()` | advances timeout state without blocking |

## Behavior

- RPM is computed as `60000000 / (pulse_interval_us * pulses_per_revolution)`.
- `timestamp_us == 0` is valid.
- Time comparisons are wrap-safe.

## Examples

- `examples-projects/xc8/sensors/tachometer.X`

## Tests

- `scripts/tests/test_tachometer.py`
