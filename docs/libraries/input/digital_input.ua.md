# digital_input

Reusable debounced digital input helper з active-high/low mapping, consuming edge flags і optional asymmetric debounce.

## API

| Елемент | Примітка |
| --- | --- |
| `digital_input_init()` | перевіряє config і задає safe initial state |
| `digital_input_update()` | подає raw levels і просуває debounce |
| `digital_input_rose()` / `digital_input_fell()` | consuming edge flags |
| `digital_input_is_latched()` | read-only latch query |

## Поведінка

- `active_level` обирає active-high або active-low mapping.
- `debounce_ms` працює через stable-time, використовується як default для обох напрямків.
- `immediate_active` перевіряється ПЕРШИМ: якщо ввімкнений, активація миттєва (0 ms) незалежно від directional полів. Це пріоритет:
  1. `immediate_active` → активація миттєва
  2. `activate_debounce_ms` / `release_debounce_ms` → directional thresholds
  3. `debounce_ms` → symmetric fallback (коли обидва directional поля 0)
- `activate_debounce_ms` та `release_debounce_ms` перезаписують symmetric debounce для кожного напрямку. Якщо обидва 0, використовується `debounce_ms` (сумісність).

### Режими debounce

**Legacy symmetric mode** (обидва directional поля = 0):
- Обидва напрямки використовують `debounce_ms`.
- Виняток: якщо `immediate_active` ввімкнений, активація все одно миттєва.
- Це backward-compatible default для існуючих конфігів.

**Asymmetric mode** (хоча б одне directional поле != 0):
- Кожен напрямок використовує власний threshold.
- `activate_debounce_ms == 0` означає миттєву активацію.
- `release_debounce_ms == 0` означає миттєвий реліз.
- Ненульове значення означає stable-time debounce для цього напрямку.
- `immediate_active` все одно перезаписує активацію на 0 ms.

- Latch опційний і скидається тільки `digital_input_clear_latch()`.

## Приклад

- `examples-projects/xc8/input/digital_input.X`

## Тести

- `scripts/tests/test_digital_input.py`
