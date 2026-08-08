# tachometer

Reusable helper для переводу імпульсів у RPM зі startup grace, noise rejection, timeout, ISR-safe critical sections і status.

## Compile-time профілі

### FULL (за замовчуванням)

Повна поведінка тахометра з розрахунком RPM, перевіркою мінімального RPM та всіма діагностичними статусами.

### LIGHTWEIGHT

Визначте `TACHOMETER_LIGHTWEIGHT=1` перед включенням заголовка для активації легкого профілю:

- Вимикає дорогий 64-бітний розподіл для розрахунку RPM
- `tachometer_get_rpm()` завжди повертає 0
- Статус TOO_SLOW ніколи не встановлюється
- Всі фільтрація імпульсів, startup grace, timeout та визначення наявності залишаються повністю функціональними
- Зберігає ~1236 B ROM на PIC18F452 (виміряно з XC8 3.10)

Використання:
```c
#define TACHOMETER_LIGHTWEIGHT 1
#include "libraries/sensors/tachometer/tachometer.h"
```

## API

| Елемент | Примітка |
| --- | --- |
| `tachometer_init()` | перевіряє config і стартує у `STOPPED` або `CONFIG_ERROR` |
| `tachometer_set_expected_running()` | явний expected-running, caller передає `now_us` |
| `tachometer_on_pulse()` | приймає timestamp імпульсу і оновлює RPM; ISR-safe |
| `tachometer_process()` | просуває timeout state без блокування |
| `tachometer_get_rpm()` | повертає snapshot поточного RPM; макро повертає 0 у LIGHTWEIGHT |
| `tachometer_get_status()` | повертає snapshot поточного status |
| `tachometer_get_pulse_count()` | повертає cumulative кількість прийнятих імпульсів |
| `tachometer_reset()` | очищає runtime state, зберігає config |

## ISR контракт

- `tachometer_on_pulse()` безпечний для виклику з таймера або зовнішнього переривання. Він використовує `DRV_INT_SAVE_AND_DISABLE` / `DRV_INT_RESTORE` з `core/compiler.h` для захисту спільних полів. Ці макроси зберігають попередній стан GIE і відновлюють його точно, тому вони безпечні з ISR контексту (де GIE вже 0) і з main loop (де GIE 1).
- `tachometer_process()` бере атомарний snapshot ISR-записаних полів (`last_pulse_us`, `session_state`, `rpm`, `expected_running`, `expected_running_since_us`) під короткою critical section. Всі розрахунки timeout та status використовують snapshot. Перед вчиненням будь-якої зміни стану (rearm, status update) функція перевіряє, що `last_pulse_us` не змінився — якщо змінився, новіший імпульс прибув і несвіжий результат відкидається.
- Гетери (`get_rpm`, `get_status`, `get_pulse_count`) повертають консистентні snapshots окремих полів, захищені короткими critical sections.
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
