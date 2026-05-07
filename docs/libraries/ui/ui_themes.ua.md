[🇬🇧 English version](./ui_themes.md)

# Бібліотека UI Themes

## Опис

`ui_themes` — легка універсальна система тем для embedded UI.
Вона задає семантичні ролі кольорів і style-підказки для іконок на монохромних і RGB565 дисплеях.

## Публічний API

- `ui_theme_get_default()`
- `ui_theme_get_dark()`
- `ui_theme_get_light()`
- `ui_theme_apply()`
- `ui_theme_get_color()`
- `ui_theme_set_color()` (опційно)
- `ui_theme_get_icon_style()` (опційно)

## Модель Теми

`ui_theme_t` містить:

- `background`
- `foreground`
- `accent`
- `warning`
- `error`
- `success`
- `disabled`
- `border`
- `icon_style`
- прапорець режиму `monochrome`

Усі ролі зберігаються як 16-бітні значення (сумісні з RGB565).

## Поведінка Monochrome vs RGB565

- RGB дисплеї: повертається оригінальний 16-бітний колір.
- Монохромні дисплеї: `ui_theme_get_color()` зводить роль до `0/1` (off/on).

Це дає єдиний API тем для OLED і TFT.

## Сумісність з Дисплеями

Система розрахована на:

- монохромний рендер через `graphics_t`
- RGB565 backend-и ST7735 / ILI9341
- інтеграцію через display framework
- рендер іконок через icon pack

## Нотатки по Використанню

- Статичні `const` теми мінімізують використання RAM.
- `ui_theme_apply()` копіює preset у runtime-тему.
- Runtime-override доступний через `ui_theme_set_color()`.

## Приклад

`example.c` демонструє:

- перемикання dark/light
- рендер themed panel
- рендер themed icons
- поведінку monochrome mapping

## Залежності

- `core/*`
- `libraries/display/graphics`
- `libraries/ui/icon_pack`

## Обмеження для Embedded

- без dynamic memory
- integer-only логіка
- малий ROM/RAM footprint
