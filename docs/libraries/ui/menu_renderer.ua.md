[🇬🇧 English version](./menu_renderer.md)

# Бібліотека Menu Renderer

## Опис

`menu_renderer` — універсальний display-agnostic шар візуалізації стану `menu_core`.
Він малює пункти меню через `graphics_t`, опційні іконки та UI-теми.

Модуль не читає hardware input і не звертається напряму до дисплейного заліза.

## Публічний API

- `menu_renderer_init()`
- `menu_renderer_draw()`
- `menu_renderer_set_theme()` (опційно)
- `menu_renderer_set_icons()` (опційно)
- `menu_renderer_set_layout()` (опційно)

## Нотатки по Рендерингу / Layout

Підтримується:

- вертикальний список меню
- підсвітка вибраного рядка
- індикатор підменю
- індикатор стану toggle
- візуалізація value-пунктів

Керування layout:

- висота рядка
- кількість видимих рядків viewport
- X/Y відступи

## Нотатки по Scrolling / Theme

Модель скролу:

- renderer веде `top_index`
- індекс вибраного пункту порівнюється з вікном viewport
- `top_index` автоматично зміщується, коли вибір виходить за межі видимої області

Інтеграція тем:

- role-based кольори з `ui_theme_t`
- підтримка mono та RGB значень тем
- поточний generic graphics path використовує non-zero/zero семантику малювання

Інтеграція іконок:

- опційний рендер item-type іконок через `icon_pack`
- стрілка підменю та state-іконки праворуч рядка

## Сумісність з Дисплеями

Renderer є display-agnostic і працює з будь-яким backend, що надає коректний `graphics_t`:

- OLED (SSD1306/SH1106 через graphics framebuffer)
- TFT (ST7735/ILI9341 через graphics callback context)
- композиції через display framework

## Приклад

`example.c` демонструє:

- themed menu rendering
- icon-based рендер пунктів
- auto-scroll поведінку
- перемикання тем
- OLED шлях та нотатки для TFT reuse

## Залежності

- `core/*`
- `libraries/ui/menu_core`
- `libraries/display/graphics`
- `libraries/display/display_framework`
- `libraries/ui/icon_pack`
- `libraries/ui/ui_themes`

## Обмеження для Embedded

- без dynamic memory
- integer-only логіка
- lightweight viewport-based рендер
