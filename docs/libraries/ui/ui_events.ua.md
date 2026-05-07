[🇬🇧 English version](./ui_events.md)

# Бібліотека UI Events

## Опис

`ui_events` — легка універсальна система подій/повідомлень для embedded UI-архітектури.
Вона незалежна від hardware та display-рівня.

## Публічний API

- `ui_events_init()`
- `ui_events_push()`
- `ui_events_pop()`
- `ui_events_peek()`
- `ui_events_clear()`
- `ui_events_subscribe()` (опційно)
- `ui_events_dispatch()` (опційно)

## Нотатки по Моделі Подій

Поля `ui_event_t`:

- `type`
- `source`
- `target`
- `param1`
- `param2`
- `user_data`

Підтримувані типи:

- `UI_EVENT_NONE`
- `UI_EVENT_INPUT`
- `UI_EVENT_MENU_CHANGED`
- `UI_EVENT_MENU_SELECTED`
- `UI_EVENT_WIDGET_CHANGED`
- `UI_EVENT_SCREEN_CHANGED`
- `UI_EVENT_POPUP_OPEN`
- `UI_EVENT_POPUP_CLOSE`
- `UI_EVENT_NOTIFICATION`

## Нотатки по Черзі / Subscriber

Модель черги:

- фіксована статична FIFO (`UI_EVENTS_QUEUE_SIZE`)
- без dynamic allocation
- політика overflow: видалення найстарішої події

Модель підписників:

- фіксований список callbacks (`UI_EVENTS_MAX_SUBSCRIBERS`)
- `ui_events_dispatch()` викликає всі зареєстровані callbacks для події

## Нотатки по Інтеграції

Система розрахована на інтеграцію з:

- `input_adapter`
- `menu_navigation`
- `menu_widgets`
- майбутніми screen manager / async сервісами

Типовий потік:

1. producer додає події в чергу
2. consumer зчитує події з черги
3. dispatcher сповіщає підписників

## Обмеження для Embedded

- без dynamic memory
- integer-only логіка
- детермінована FIFO поведінка

## Залежності

- `core/*`
- `libraries/ui/menu_core`
- `libraries/ui/menu_navigation`
- `libraries/ui/menu_widgets`
- `libraries/ui/input_adapter`
