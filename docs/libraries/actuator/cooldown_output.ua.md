# cooldown_output

Reusable requested/active output helper з cooldown delay, remaining time і optional callback.

## API

| Елемент | Примітка |
| --- | --- |
| `cooldown_output_init()` | стартує вимкненим |
| `cooldown_output_set_requested()` | request on/off з caller-переданим `now_ms` |
| `cooldown_output_process()` | завершує cooldown після deadline |
| `cooldown_output_get_remaining_ms()` | wrap-safe запит remaining time |

## Поведінка

- `cooldown_ms == 0` означає immediate off.
- Повторний request on скасовує pending shutdown.
- Callback спрацьовує лише на зміну active-state.
- Для коректності wrap-safe порівняння deadline `cooldown_ms` має бути меншим за `2^31` мс. Вихід обов'язково треба політи через `cooldown_output_process()`.

## Приклад

- `examples-projects/xc8/actuator/cooldown_output.X`

## Тести

- `scripts/tests/test_cooldown_output.py`
