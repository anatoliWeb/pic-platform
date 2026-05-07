[🇬🇧 English version](./sprite_buffer.md)

# Бібліотека Sprite Buffer

## Опис

`sprite_buffer` — універсальний offscreen шар для спрайтів/буферів кадру у вбудованому UI.
Модуль display-agnostic і інтегрується з `graphics` та `bitmap`.

## Публічний API

- `sprite_init()`
- `sprite_clear()`
- `sprite_draw_pixel()`
- `sprite_fill_rect()` (опційно)
- `sprite_draw_bitmap()`
- `sprite_draw_sprite()`
- `sprite_copy_region()` (опційно)
- `sprite_blit()`
- `sprite_blit_transparent()` (опційно)

## Нотатки по Sprite Framebuffer

`sprite_t` містить:

- `width`, `height`
- `format`
- `framebuffer`
- `framebuffer_size`

Поточний формат:

- `SPRITE_FORMAT_1BPP_ROW`

Модель зберігання:

- row-based packing
- `bytes_per_row = (width + 7) / 8`
- порядок бітів у байті: MSB-first

## Нотатки по Рендерингу і Blit

- Усе малювання виконується у локальному sprite framebuffer (offscreen).
- `sprite_blit()` копіює ненульові пікселі спрайта у цільовий `graphics_t`.
- `sprite_blit_transparent()` пропускає задане transparent bit значення.
- Безпечне clipping-зрізання забезпечується через bounds checks у `graphics_draw_pixel()`.

## Модель Offscreen Rendering

Типовий сценарій:

1. Рендер елемента UI у sprite buffer
2. Композиція/копіювання регіонів спрайта
3. Blit спрайта у фінальну graphics-поверхню
4. Flush дисплея на рівні display backend

Підходить для буферизації анімацій та побудови UI-панелей.

## Залежності

- `core/*`
- `libraries/display/graphics`
- `libraries/display/bitmap`

## Обмеження для Embedded

- Без dynamic memory
- Лише integer-логіка
- Легкі цикли та RAM-friendly структура
