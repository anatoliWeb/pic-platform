[English version](./shared_segment_buttons.md)

# Бібліотека shared-кнопок для seven-segment

## Опис
Бібліотека для опитування кнопок, які ділять IO-лінії з мультиплексним seven-segment дисплеєм.
Орієнтована на low pin-count embedded системи.

## Публічний API
- `shared_segment_buttons_init()`
- `shared_segment_buttons_update()`
- `shared_segment_buttons_is_pressed()`
- `shared_segment_buttons_get_key()`

## Модель shared IO
Використовується наявна розводка дисплея:
- спільні сегментні лінії
- лінії вибору розряду

Кожна кнопка задається трійкою:
- індекс розряду
- сегментна лінія
- key code

## Неблокуюча модель сканування
`shared_segment_buttons_update()` за один виклик сканує лише одну кнопку:
1. коротко вимкнути всі розряди
2. перевести потрібний сегмент у input
3. увімкнути потрібну лінію розряду
4. зчитати shared-лінію
5. вимкнути розряд і повернути сегмент в output

Внутрішніх blocking-delay немає.

## Взаємодія з мультиплексом
- Бібліотека працює паралельно з `seven_segment_mux_update()`.
- Рекомендований порядок викликів у циклі:
  1. `seven_segment_mux_update()`
  2. `shared_segment_buttons_update()`
- Коротке вікно зчитування мінімізує мерехтіння.

## Модель конфігурації
`shared_segment_buttons_config_t`:
- вказівник `display` (`seven_segment_mux_t`)
- масив мапінгу `map`
- `button_count`
- `active_level`
- `debounce_scans`

## Нотатки по підключенню
- Схема кнопок має бути спроєктована під shared-segment топологію.
- Часто використовують pull-up/pull-down і діодну розв'язку за потреби.
- Дотримуйтесь електричних обмежень ліній сегментів і розрядів.

## Обмеження
- За один `update()` сканується лише одна кнопка.
- Debounce базується на кількості сканів, а не на часу.
- Некоректна зовнішня схема може викликати хибні натискання.

## Залежності
- `core/*`
- `drivers/gpio/*`
- `libraries/display/seven_segment_mux/*`