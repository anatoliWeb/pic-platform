# digital_input

Reusable debounced digital input helper з active-high/low mapping і consuming edge flags.

## API

| Елемент | Примітка |
| --- | --- |
| `digital_input_init()` | перевіряє config і задає safe initial state |
| `digital_input_update()` | подає raw levels і просуває debounce |
| `digital_input_rose()` / `digital_input_fell()` | consuming edge flags |
| `digital_input_is_latched()` | read-only latch query |

## Поведінка

- `active_level` обирає active-high або active-low mapping.
- `debounce_ms` працює через stable-time.
- Latch опційний і скидається тільки `digital_input_clear_latch()`.

## Приклад

- `examples-projects/xc8/input/digital_input.X`

## Тести

- `scripts/tests/test_digital_input.py`
