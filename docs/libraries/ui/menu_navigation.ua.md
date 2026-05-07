[🇬🇧 English version](./menu_navigation.md)

# Бібліотека Menu Navigation

## Опис

`menu_navigation` — легкий контролерний шар, що з'єднує `input_adapter` та `menu_core`.
Він перетворює уніфіковані input-події у детерміновані дії навігації меню.

Модуль не залежить від display/rendering систем.

## Публічний API

- `menu_navigation_init()`
- `menu_navigation_update()`
- `menu_navigation_set_enabled()` (опційно)
- `menu_navigation_reset()` (опційно)

## Нотатки по Навігаційному Потоку

`menu_navigation_t` зв'язує:

- `menu_t* menu`
- `input_adapter_t* input`
- прапорець `enabled`

`menu_navigation_update()` виконує:

1. polling через `input_adapter_update()`
2. цикл зчитування подій з черги
3. мапінг подій у menu-операції

Блокуюча логіка або затримки відсутні.

## Нотатки по Мапінгу Подій

Реалізований мапінг:

- `INPUT_EVENT_UP` -> `menu_prev()`
- `INPUT_EVENT_DOWN` -> `menu_next()`
- `INPUT_EVENT_SELECT` -> `menu_enter()`
- `INPUT_EVENT_BACK` -> `menu_back()`
- `INPUT_EVENT_HOLD` -> `menu_back()` (швидкий вихід за замовчуванням)

Опційний мапінг у поточній версії:

- `INPUT_EVENT_LEFT` -> `menu_prev()`
- `INPUT_EVENT_RIGHT` -> `menu_next()`

Це дає просту навігацію value/toggle елементів при збереженні backend-agnostic підходу.

## Обробка Value / Toggle

Логіка toggle/value виконується всередині `menu_core` через `menu_enter()`.
Контролерний шар залишається мінімальним і перевикористовуваним.

## Приклад

`example.c` демонструє:

- ієрархічний стан меню
- інтеграцію encoder/button/keypad через input adapter
- поведінку toggle/value пунктів
- шлях виконання action callback

## Залежності

- `core/*`
- `libraries/ui/menu_core`
- `libraries/ui/input_adapter`

## Обмеження для Embedded

- без dynamic memory
- integer-only логіка
- фіксована детермінована обробка подій
