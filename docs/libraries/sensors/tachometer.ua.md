# tachometer

Reusable helper для переводу імпульсів у RPM зі startup grace, noise rejection, timeout і status.

## API

| Елемент | Примітка |
| --- | --- |
| `tachometer_init()` | перевіряє config і стартує у `STOPPED` або `CONFIG_ERROR` |
| `tachometer_set_expected_running()` | явний expected-running, caller передає `now_us` |
| `tachometer_on_pulse()` | приймає timestamp імпульсу і оновлює RPM |
| `tachometer_process()` | просуває timeout state без блокування |

## Поведінка

- RPM рахується як `60000000 / (pulse_interval_us * pulses_per_revolution)`.
- `timestamp_us == 0` є валідним.
- Порівняння часу wrap-safe.

## Приклад

- `examples-projects/xc8/sensors/tachometer.X`

## Тести

- `scripts/tests/test_tachometer.py`
