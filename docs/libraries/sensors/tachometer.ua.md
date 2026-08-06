# tachometer

Reusable helper для переводу імпульсів у RPM зі startup grace, noise rejection, timeout, ISR-safe critical sections і status.

## API

| Елемент | Примітка |
| --- | --- |
| `tachometer_init()` | перевіряє config і стартує у `STOPPED` або `CONFIG_ERROR` |
| `tachometer_set_expected_running()` | явний expected-running, caller передає `now_us` |
| `tachometer_on_pulse()` | приймає timestamp імпульсу і оновлює RPM; ISR-safe |
| `tachometer_process()` | просуває timeout state без блокування |
| `tachometer_get_rpm()` | повертає snapshot поточного RPM |
| `tachometer_get_status()` | повертає snapshot поточного status |
| `tachometer_get_pulse_count()` | повертає cumulative кількість прийнятих імпульсів |
| `tachometer_reset()` | очищає runtime state, зберігає config |

## ISR контракт

- `tachometer_on_pulse()` безпечний для виклику з таймера або зовнішнього переривання. Він використовує `TACHOMETER_CRITICAL_ENTER/EXIT` для захисту спільних полів.
- `tachometer_process()` має викликатися з main loop. Він також використовує critical sections при читанні полів, що записуються ISR.
- Гетери (`get_rpm`, `get_status`, `get_pulse_count`) повертають консистентні snapshots.
- `init`, `set_expected_running` та `reset` призначені лише для main loop.

## Поведінка

- RPM рахується як `60000000 / (pulse_interval_us * pulses_per_revolution)`.
- `timestamp_us == 0` є валідним.
- Порівняння часу wrap-safe.
- Для розрахунку RPM потрібні два імпульси; перший імпульс сесії лише re-arm measurement state.
- Після `signal_timeout_ms` без імпульсу сесія re-arm: RPM стає 0, старий timestamp імпульсу більше не використовується, наступний імпульс починає свіжу сесію. Cumulative `pulse_count` при timeout не скидається.
- `pulse_count` рахує прийняті імпульси з моменту init, reset або зміни expected-running; він переживає timeouts.
- `session_state` є іменованою фазою measurement session (`TACHOMETER_SESSION_UNARMED` / `FIRST_PULSE` / `ACTIVE`), а не cumulative counter.
- Довгі потоки імпульсів насичують `session_state` на `ACTIVE`; після другого accepted pulse він лишається в цій фазі. Cumulative `pulse_count` продовжує рости.

## Приклад

- `examples-projects/xc8/sensors/tachometer.X`

## Тести

- `scripts/tests/test_tachometer.py`
