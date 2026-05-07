[🇬🇧 English version](./menu_core.md)

# Бібліотека Menu Core

## Опис

`menu_core` — універсальне ядро дерева меню та навігації для embedded.
Воно повністю незалежне від рендерингу та апаратного вводу.

Основні задачі ядра:

- структура меню
- стан вибору
- стан навігації
- callbacks дій

## Публічний API

- `menu_init()`
- `menu_set_root()`
- `menu_get_current()`
- `menu_next()`
- `menu_prev()`
- `menu_enter()`
- `menu_back()`
- `menu_select()` (опційно)
- `menu_reset()` (опційно)

## Нотатки по Структурі Меню

`menu_item_t` підтримує:

- `parent`
- `first_child`
- сусіди `next` / `prev`
- `label`
- `flags`
- callback дії + `action_ctx`
- поля даних для toggle/value пунктів

Підтримувані типи пунктів:

- `MENU_ITEM_ACTION`
- `MENU_ITEM_SUBMENU`
- `MENU_ITEM_TOGGLE`
- `MENU_ITEM_VALUE`

## Нотатки по Моделі Навігації

Стан меню (`menu_t`) зберігає:

- `root`
- `current` (поточний контекст підменю/кореня)
- `selected` (поточний вибраний пункт)

Поведение навігації:

- `menu_next/menu_prev` переміщаються по sibling-ланцюгу
- `menu_enter` відкриває submenu або виконує action/toggle/value
- `menu_back` повертає до батьківського меню та відновлює вибір

## Модель Статичної Алоцікації

Дерево меню розраховане на статичне розміщення:

- елементи меню оголошуються static/global
- зв'язки між ними задаються на старті
- dynamic allocation не потрібен

## Модель Callback

Тип callback дії:

- `typedef void (*menu_action_cb_t)(void* ctx);`

Дозволяє викликати UI/application дії без залежності від display/input шарів.

## Приклад

`example.c` демонструє:

- ієрархічне дерево меню
- вхід у підменю та повернення назад
- поведінку toggle/value пунктів
- використання action callback

## Залежності

- `core/*`

## Обмеження для Embedded

- без dynamic memory
- integer-only логіка
- детермінований pointer-based traversal
