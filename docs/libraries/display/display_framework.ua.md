# Display Framework

## Огляд

`display_framework` надає легкий, переносимий шар абстракції дисплеїв для embedded-проєктів.
Він уніфікує high-level API малювання та делегує рендеринг backend-адаптерам.

Це не GUI-фреймворк, а тонкий portability layer.

## Публічний API

- `display_init()`
- `display_clear()`
- `display_update()`
- `display_draw_pixel()`
- `display_draw_char()`
- `display_draw_string()`
- `display_draw_number()`
- `display_draw_line()` (опційно)
- `display_draw_rect()` (опційно)

## Модель backend-абстракції

Framework використовує backend через function pointers:

- `clear(ctx)`
- `update(ctx)`
- `draw_pixel(ctx, x, y, color)`
- `draw_char(ctx, x, y, char, color)`

`display_t` зберігає callbacks, вказівник на backend context і розмір дисплея.

## Підтримані backend-и (початково)

- LCD HD44780 адаптер
- Seven Segment адаптер (single-digit стиль)
- WS2812 Matrix адаптер

Якщо операція не підтримується конкретним дисплеєм, адаптер виконує no-op.

## Модель координат

Єдина система координат: `x, y`.
Backend може ігнорувати непідтримувані можливості (наприклад seven-segment).

## Рендеринг тексту

- `display_draw_string()` і `display_draw_number()` єдині для всіх backend-ів.
- Для matrix використовується легкий вбудований 5x7 набір гліфів (цифри та базові символи).
- Для LCD символи напряму делегуються драйверу LCD.

## Модель пам'яті

- Без dynamic memory.
- Framework не володіє framebuffer.
- Context backend-а вказує на вже існуючі об'єкти дисплея.

## Цілі переносимості

- Один API для різних типів дисплеїв.
- Легке перемикання backend без переписування прикладної логіки.
- Працює в main loop, scheduler task або RTOS task.

## Нотатки для PIC18

- Тримайте draw-операції легкими.
- Налаштовуйте частоту update для конкретного backend-а.
- Уникайте важких full-screen redraw циклів і великих шрифтів.
