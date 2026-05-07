[🇬🇧 English version](./input_adapter.md)

# Бібліотека Input Adapter

## Опис

`input_adapter` — універсальний шар абстракції вводу для embedded UI/навігації.
Бібліотека перетворює події конкретних input-модулів у єдиний формат UI-подій.

Модуль не залежить від display/rendering систем.

## Публічний API

- `input_adapter_init()`
- `input_adapter_update()`
- `input_adapter_get_event()`
- `input_adapter_clear()` (опційно)
- `input_adapter_peek()` (опційно)

## Модель Подій Вводу

Підтримувані події:

- `INPUT_EVENT_NONE`
- `INPUT_EVENT_UP`
- `INPUT_EVENT_DOWN`
- `INPUT_EVENT_LEFT`
- `INPUT_EVENT_RIGHT`
- `INPUT_EVENT_SELECT`
- `INPUT_EVENT_BACK`
- `INPUT_EVENT_HOLD`

## Підтримувані Джерела Вводу

- бібліотека `button`
- бібліотека `encoder` (включно з кнопкою енкодера)
- бібліотека `keypad_matrix`

Можна використовувати кілька джерел одночасно.

## Нотатки по Буферизації / Трансляції

- Модель оновлення: polling.
- Легка фіксована FIFO-черга (`INPUT_ADAPTER_QUEUE_SIZE`).
- Політика переповнення: видалення найстарішої події.
- Символи keypad транслюються через поля мапінгу в адаптері.

Мапінг keypad за замовчуванням:

- up: `'2'`
- down: `'8'`
- left: `'4'`
- right: `'6'`
- select: `'#'`
- back: `'*'`

## Потік Подій

1. `input_adapter_update()` опитує всі підключені джерела.
2. Стан джерел перекладається у `input_event_t`.
3. Події додаються в чергу.
4. Застосунок читає події через `input_adapter_get_event()`.

## Обмеження для Embedded

- без dynamic memory
- integer-only логіка
- малий RAM footprint

## Залежності

- `core/*`
- `libraries/input/button`
- `libraries/input/encoder`
- `libraries/input/keypad_matrix`
