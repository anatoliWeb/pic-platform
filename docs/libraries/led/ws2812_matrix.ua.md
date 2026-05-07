# Бібліотека WS2812 Matrix

## Опис

`ws2812_matrix` — це легкий 2D-шар мапінгу для WS2812/WS2812B.  
Бібліотека перетворює координати `(x, y)` у лінійний індекс LED та використовує існуючий драйвер WS2812 для виводу.

## Публічний API

- `ws2812_matrix_init()`
- `ws2812_matrix_get_index()`
- `ws2812_matrix_set_pixel()`
- `ws2812_matrix_clear()`
- `ws2812_matrix_fill()`
- `ws2812_matrix_show()`
- `ws2812_matrix_draw_line()` (опційний helper)
- `ws2812_matrix_draw_rect()` (опційний helper)

## Підтримувані схеми матриці

- Row-major
- Column-major
- Serpentine (zigzag) або пряме підключення
- Кут початку:
  - top-left
  - top-right
  - bottom-left
  - bottom-right
- Опційний reverse по всьому ланцюгу

## Модель мапінгу

1. Перевірка меж `(x, y)`.
2. Трансформація координат за обраним кутом початку.
3. Обчислення базового індексу (row-major або column-major).
4. Для serpentine: інверсія позиції на непарних рядках/колонках.
5. За потреби: reverse індексу.

## Приклад використання

```c
ws2812_matrix_config_t cfg;
cfg.strip = &strip;
cfg.width = 8;
cfg.height = 8;
cfg.layout = WS2812_MATRIX_LAYOUT_ROW_MAJOR;
cfg.serpentine = 1;
cfg.origin = WS2812_MATRIX_ORIGIN_TOP_LEFT;
cfg.reverse = 0;

ws2812_matrix_init(&matrix, &cfg);
ws2812_matrix_set_pixel(&matrix, 3, 2, rgb_color(255, 0, 0));
ws2812_matrix_show(&matrix);
```

## Залежності

- `core/*`
- `libraries/led/ws2812`
- `libraries/led/rgb_utils`

## Примітки по RAM і продуктивності

- Без dynamic memory.
- Використовує наявний WS2812 framebuffer.
- Лише цілочисельна математика, без float.
- Таймінги передачі залишаються в low-level WS2812 реалізації.

## Обмеження

- Розмір матриці обмежений RAM PIC18 (буфер пікселів).
- Бібліотека не містить шрифти/спрайти, лише 2D-мапінг і прості draw helper-и.
