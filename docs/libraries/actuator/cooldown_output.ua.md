# cooldown_output

Reusable requested/active output helper з cooldown delay, remaining time і optional callback.

## API

| Елемент | Примітка |
| --- | --- |
| `cooldown_output_init()` | стартує вимкненим, зберігає початковий cooldown_ms |
| `cooldown_output_set_requested()` | request on/off з caller-переданим `now_ms` |
| `cooldown_output_set_duration_ms()` | зміна cooldown в runtime; повертає помилку при недійсній тривалості |
| `cooldown_output_cancel()` | скасування pending cooldown, синхронізація requested з active |
| `cooldown_output_process()` | завершує cooldown після deadline |
| `cooldown_output_get_remaining_ms()` | wrap-safe запит remaining time |
| `cooldown_output_force_off()` | примусове вимикання негайно |

## Константи

| Елемент | Примітка |
| --- | --- |
| `COOLDOWN_OUTPUT_MAX_DURATION_MS` | максимальна валідна тривалість (0x7FFFFFFF); wrap-safe ліміт |

## Поведінка

- `cooldown_ms == 0` означає immediate off.
- Повторний request on скасовує pending shutdown.
- Callback спрацьовує лише на зміну active-state.
- `set_duration_ms()` повертає `DRV_STATUS_ERROR` якщо тривалість перевищує `COOLDOWN_OUTPUT_MAX_DURATION_MS`. Якщо відбувається охолодження, deadline перераховується з `now_ms` з новою тривалістю. Якщо нова тривалість 0, вихід вимикається негайно.
- `cancel()` очищає прапор cooling-down і синхронізує `requested` з `active`: якщо вихід ON, `requested` стає 1; якщо OFF, `requested` лишається 0. Це запобігає суперечливому стану де `requested=0` але `active=1` без cooldown.
- Для коректності wrap-safe порівняння deadline `cooldown_ms` має бути меншим за `2^31` мс. Вихід обов'язково треба політи через `cooldown_output_process()`.

## Приклад

- `examples-projects/xc8/actuator/cooldown_output.X`

## Тести

- `scripts/tests/test_cooldown_output.py`
