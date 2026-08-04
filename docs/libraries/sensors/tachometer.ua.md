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
- Для розрахунку RPM потрібні два імпульси; перший імпульс сесії лише re-arm measurement state.
- Після `signal_timeout_ms` без імпульсу сесія re-arm: RPM стає 0, старий timestamp імпульсу більше не використовується, наступний імпульс починає свіжу сесію. Cumulative `pulse_count` при timeout не скидається.
- `pulse_count` рахує прийняті імпульси з моменту init, reset або зміни expected-running; він переживає timeouts.
- `session_pulse_count` є насиченим measurement state: `0` = unarmed, `1` = first pulse, `2` = active measurement.
- Довгі потоки імпульсів не переповнюють `session_pulse_count`; після другого accepted pulse він лишається на `2`.

## Приклад

- `examples-projects/xc8/sensors/tachometer.X`

## Тести

- `scripts/tests/test_tachometer.py`
