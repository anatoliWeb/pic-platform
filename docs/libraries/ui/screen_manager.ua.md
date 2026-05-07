[🇬🇧 English version](./screen_manager.md)

# Бібліотека Screen Manager

## Опис

`screen_manager` — легкий універсальний менеджер життєвого циклу screen/page для embedded UI.
Він не залежить від display чи input hardware.

Модуль керує:

- активним екраном
- modal push/pop поведінкою стека
- callbacks оновлення/рендерингу
- пересилкою подій у активний екран

## Публічний API

- `screen_manager_init()`
- `screen_manager_set()`
- `screen_manager_push()`
- `screen_manager_pop()`
- `screen_manager_update()`
- `screen_manager_draw()`
- `screen_manager_send_event()` (опційно)
- `screen_manager_get_current()` (опційно)

## Нотатки по Життєвому Циклу Screen

Поля `screen_t`:

- `id`
- `user_data`
- `on_enter()`
- `on_exit()`
- `on_update()`
- `on_draw()`
- `on_event()`

Потік lifecycle:

- `set`: exit поточного -> заміна root у стеку -> enter нового
- `push`: push modal screen -> enter нового top
- `pop`: exit top -> повернення до попереднього top (`on_enter`)
- `update`: update активного + форвардинг queued events
- `draw`: callback рендерингу активного екрану

## Нотатки по Stack / Event Flow

Модель стека:

- фіксований статичний стек (`SCREEN_MANAGER_STACK_SIZE`)
- детермінована push/pop поведінка
- без dynamic allocation

Інтеграція з подіями:

- опційна прив'язка `ui_events_t*` у менеджері
- `screen_manager_update()` читає події з черги та передає активному екрану
- `screen_manager_send_event()` дає direct event injection

## Modal Поведінка

- `push` призначено для popup/dialog overlay
- `pop` повертає попередній контекст
- архітектура готова до overlay/popup manager розширень

## Залежності

- `core/*`
- `libraries/ui/ui_events`
- `libraries/ui/menu_renderer`
- `libraries/display/graphics`
- `libraries/display/display_framework`

## Обмеження для Embedded

- без dynamic memory
- integer-only логіка
- детерміноване виконання callbacks
