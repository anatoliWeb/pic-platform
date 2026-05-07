[🇬🇧 English version](./graphics.md)

# Бібліотека Graphics Core

## Опис

`graphics` — універсальне display-agnostic ядро рендерингу для embedded-дисплеїв.
Призначене для спільного використання з SSD1306, SH1106 та майбутніми backend-ами.

## Публічний API

- `graphics_init()`
- `graphics_draw_pixel()`
- `graphics_draw_line()`
- `graphics_draw_rect()`
- `graphics_fill_rect()`
- `graphics_draw_circle()`
- `graphics_draw_char()`
- `graphics_draw_string()`
- `graphics_draw_triangle()` (опційно)
- `graphics_fill_circle()` (опційно)
- `graphics_flush()`

## Модель Контексту Graphics

`graphics_t` містить:

- `width`, `height`
- pointer на framebuffer + його розмір
- опційні callbacks для пікселя (`set/get`)
- опційний callback для flush
- user context pointer

Це дозволяє працювати як напряму з framebuffer, так і через кастомний backend.

## Алгоритми Рендерингу

- Лінія: алгоритм Bresenham
- Коло: midpoint circle algorithm
- Прямокутники/fill: легка integer rasterization
- Шрифт: вбудований ASCII 5x7

Використовується лише integer-математика.

## Clipping та Безпека

- Кожен піксель проходить перевірку меж.
- Малювання за межами екрану безпечно відсікається.
- Захист від виходу за межі framebuffer.

## Інтеграція з Дисплеями

- Ядро graphics не виконує доступ до hardware шин.
- Працює з framebuffer SSD1306 через `framebuffer` + `flush_cb`.
- Підготовлено для SH1106 та інших майбутніх backend-ів.

## Приклад

`example.c` демонструє:

- рендеринг тексту
- рендеринг примітивів
- просту анімацію
- інтеграцію з SSD1306 через framebuffer + flush callback

## Залежності

- `core/*`
- опційна інтеграція з `libraries/display/display_framework`

## Обмеження для Embedded

- Без dynamic memory
- Лише integer-математика
- Легкі алгоритми і невеликий стан
