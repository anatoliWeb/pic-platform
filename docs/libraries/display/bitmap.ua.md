[🇬🇧 English version](./bitmap.md)

# Бібліотека Bitmap Renderer

## Опис

`bitmap` — універсальний display-agnostic модуль рендерингу бітмапів для graphics-системи pic-platform.
Модуль працює через `graphics_t` і не виконує прямий доступ до hardware.

## Публічний API

- `bitmap_draw()`
- `bitmap_draw_transparent()`
- `bitmap_draw_region()`
- `bitmap_draw_scaled()` (опційно)
- `bitmap_draw_inverted()` (опційно)

## Модель Bitmap

Дескриптор `bitmap_t` містить:

- `width`
- `height`
- `format`
- pointer на `data`

Поточний формат:

- `BITMAP_FORMAT_1BPP_ROW` (монохромний, row-based packing)

Архітектура підготовлена для розширення:

- grayscale формати
- RGB формати
- compressed формати

## Нотатки по 1bpp Row Format

Для `BITMAP_FORMAT_1BPP_ROW`:

- `bytes_per_row = (width + 7) / 8`
- порядок бітів у байті: MSB first (`0x80 >> bit_index`)
- зберігання row-major

## Рендеринг і Clipping

- Усі записи пікселів проходять через `graphics_draw_pixel()`.
- Clipping виконується через bounds checks у graphics core.
- `bitmap_draw_region` зручний для sprite-sheet рендерингу.

## Прозорість

`bitmap_draw_transparent()` підтримує transparent bits:

- `transparent_bit = 0` або `1`
- пікселі з цим значенням пропускаються

## Нотатки по Продуктивності

- Лише integer-математика
- Без dynamic memory
- Легкі цикли для embedded-систем
- Масштабування реалізовано через nearest-neighbor replication

## Приклад

`example.c` демонструє:

- рендеринг іконки
- transparent overlay
- clipped region рендер
- scaled/inverted рендер
- просту bitmap-анімацію

## Залежності

- `core/*`
- `libraries/display/graphics`
