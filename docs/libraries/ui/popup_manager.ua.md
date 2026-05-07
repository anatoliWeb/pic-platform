[🇬🇧 English version](./popup_manager.md)

# Бібліотека Popup Manager

## Опис

`popup_manager` — універсальний менеджер modal popup/dialog для embedded UI.
Він display-agnostic та input-agnostic.

Модуль підтримує один активний popup і опціональну чергу відкладених popup.

## Публічний API

- `popup_manager_init()`
- `popup_show()`
- `popup_close()`
- `popup_update()`
- `popup_draw()`
- `popup_is_active()` (опційно)
- `popup_get_result()` (опційно)

## Нотатки по Popup / Modal Flow

Модель popup (`popup_t`) містить:

- `type`
- `title`
- `message`
- `icon`
- `buttons`
- `result`
- `timeout_ms`
- `user_data`

Підтримувані типи popup:

- info
- warning
- error
- confirmation
- notification

Modal-поведінка:

- активний лише один popup
- додаткові popup можуть ставитись у фіксовану чергу
- після закриття активного popup може автоматично відкриватись наступний

## Нотатки по Overlay / Event

Overlay-рендеринг використовує:

- `graphics_t`
- `ui_theme_t`
- `icon_pack`

Прямий hardware доступ відсутній.

Інтеграція подій:

- генерує `UI_EVENT_POPUP_OPEN`
- генерує `UI_EVENT_POPUP_CLOSE`
- споживає `UI_EVENT_INPUT` для навігації/вибору кнопок
- timeout-закриття повертає `POPUP_RESULT_TIMEOUT`

Мапінг input-подій у popup:

- LEFT/RIGHT -> перемикання кнопки
- SELECT -> підтвердження обраної кнопки
- BACK/HOLD -> скасування popup

## Timeout Handling

- timeout стартує при першому `popup_update()`
- коли `(now_ms - started_ms) >= timeout_ms`, popup закривається з timeout-результатом

## Детермінована Модель Пам'яті

- фіксована черга popup (`POPUP_QUEUE_SIZE`)
- лише статичні дані
- без dynamic memory

## Залежності

- `core/*`
- `libraries/ui/screen_manager`
- `libraries/ui/ui_events`
- `libraries/display/graphics`
- `libraries/ui/ui_themes`
- `libraries/ui/icon_pack`

## Приклад

`example.c` демонструє:

- info popup
- warning popup з timeout
- confirmation dialog
- queue popup-ів
- popup draw/update цикл
