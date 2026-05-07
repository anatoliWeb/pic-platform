[🇬🇧 English version](./icon_pack.md)

# Бібліотека Icon Pack

## Опис

`icon_pack` — універсальна бібліотека вбудованих іконок для UI-шару pic-platform.
Вона display-agnostic і працює через модулі `graphics` та `bitmap`.

## Публічний API

- `icon_get()`
- `icon_draw()`
- `icon_draw_scaled()` (опційно)
- `icon_draw_inverted()` (опційно)

## Нотатки про Формат / Зберігання Іконок

`ui_icon_t` містить:

- `width`
- `height`
- `format`
- pointer на `data`

Поточний формат:

- монохромний `BITMAP_FORMAT_1BPP_ROW`

Модель зберігання:

- іконки зберігаються як `static const`
- оптимізовано під flash/ROM
- без runtime-алокацій

## Реалізовані Групи Іконок

System:

- settings
- warning
- error
- info

Connectivity:

- wifi
- bluetooth
- ethernet

Status:

- battery
- charging
- temperature
- clock

Controls:

- play
- stop
- pause
- menu
- back
- arrows (up/down/left/right)

## Нотатки по Інтеграції Рендерингу

- `icon_draw()` використовує bitmap renderer (`bitmap_draw_transparent`).
- Ціль рендерингу — контекст `graphics_t`.
- Clipping виконується через bounds checks у graphics.
- Hardware-specific коду в icon pack немає.

## Приклад

`example.c` демонструє:

- рендеринг сітки іконок
- OLED demo (SSD1306 + graphics)
- TFT demo (ST7735 через bitmap path)
- просту анімацію іконок

## Залежності

- `core/*`
- `libraries/display/bitmap`
- `libraries/display/graphics`

## Обмеження для Embedded

- без dynamic memory
- integer-only логіка
- ROM-friendly static assets
