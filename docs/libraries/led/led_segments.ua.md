# Бібліотека LED Segments

## Опис

`led_segments` надає логічні області LED поверх існуючого WS2812 strip або WS2812 matrix.
Це дозволяє робити окремі UI-зони (status bar, progress bar, анімаційні області) без додаткових буферів.

## Публічний API

- `led_segment_init()`
- `led_segment_set_pixel()`
- `led_segment_fill()`
- `led_segment_clear()`
- `led_segment_show()`
- `led_segment_reverse()` (опційний helper)
- `led_segment_mirror()` (опційний helper)

## Модель мапінгу сегмента

- Сегмент є логічним view над батьківським об'єктом:
  - лінійна ділянка strip (`start_index`, `length`)
  - ділянка matrix (`region_x`, `region_y`, `region_width`, `region_height`)
- Логічний індекс `0..length-1` перетворюється у фізичний LED.
- Прапор `reverse` інвертує напрямок логічної адресації.

## Модель спільного framebuffer

- Без дублювання пам'яті пікселів.
- Сегмент пише напряму у буфер strip/matrix.
- `led_segment_show()` делегує вивід у `ws2812_show()` або `ws2812_matrix_show()`.

## Приклади використання

- Кілька status зон на одній стрічці.
- Segmented progress bar.
- Дзеркальні лівий/правий регіони.
- Підобласті матриці для незалежних віджетів.

## Залежності

- `core/*`
- `libraries/led/ws2812`
- `libraries/led/rgb_utils`
- `libraries/led/ws2812_matrix` (для режиму matrix region)

## Примітки по продуктивності

- Без dynamic memory.
- Без blocking delays.
- Лише integer math.
- Підходить для інкрементальних оновлень у loop/scheduler/RTOS task.
