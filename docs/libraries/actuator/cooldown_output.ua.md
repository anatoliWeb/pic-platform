# cooldown_output

Reusable requested/active output helper з cooldown delay, remaining time і optional callback.

## API

| Елемент | Примітка |
| --- | --- |
| `cooldown_output_init()` | стартує вимкненим, зберігає початковий cooldown_ms |
| `cooldown_output_set_requested()` | request on/off з caller-переданим `now_ms` |
| `cooldown_output_set_duration_ms()` | зміна cooldown в runtime; перераховує deadline при охолодженні |
| `cooldown_output_cancel()` | скасування pending cooldown, збереження поточного active state |
| `cooldown_output_process()` | завершує cooldown після deadline |
| `cooldown_output_get_remaining_ms()` | wrap-safe запит remaining time |
| `cooldown_output_force_off()` | примусове вимикання негайно |

## Поведінка

- `cooldown_ms == 0` означає immediate off.
- Повторний request on скасовує pending shutdown.
- Callback спрацьовує лише на зміну active-state.
- `set_duration_ms()` оновлює збережений cooldown. Якщо відбувається охолодження, deadline перераховується з `now_ms` з новою тривалістю. Якщо нова тривалість 0, вихід вимикається негайно.
- `cancel()` очищає прапор cooling-down, зберігаючи поточний фізичний стан виходу.
- Для коректності wrap-safe порівняння deadline `cooldown_ms` має бути меншим за `2^31` мс. Вихід обов'язково треба політи через `cooldown_output_process()`.

## Приклад

- `examples-projects/xc8/actuator/cooldown_output.X`

## Тести

- `scripts/tests/test_cooldown_output.py`
