# LED Animation Engine

## Опис

`led_animation_engine` — неблокуючий runtime-шар для керування кількома LED анімаціями одночасно.
Працює через fixed-size pool і підтримує цілі типу segment та matrix.

## Публічний API

- `led_animation_engine_init()`
- `led_animation_engine_add()`
- `led_animation_engine_remove()`
- `led_animation_engine_update()`
- `led_animation_engine_pause()`
- `led_animation_engine_resume()`
- `led_animation_engine_is_running()` (опційний helper)

## Модель рушія

- Фіксований пул анімацій (`LED_ANIM_ENGINE_MAX_ANIMATIONS`).
- Кожна анімація містить:
  - target (`segment` або `matrix`)
  - тип ефекту
  - швидкість у тиках
  - колір
  - стани enabled/paused
  - компактний runtime state (index, direction, level, hue).
- Без dynamic memory.

## Модель оновлення

Кожен виклик `led_animation_engine_update()`:

1. Проходить по активних анімаціях.
2. Збільшує локальні tick-лічильники.
3. Виконує крок ефекту, коли `tick_counter >= speed_ticks`.
4. Оновлює пікселі в цільовому буфері.
5. За потреби викликає show для target (`auto_show`).

Підходить для main loop, scheduler, RTOS task.

## Реалізовані ефекти

- Blink
- Breathing
- Rainbow
- Color wipe
- Scanner
- Pulse

## Модель пам'яті

- Використовується спільний framebuffer strip/matrix.
- Без дублювання буферів.
- Runtime state малий і передбачуваний.

## Залежності

- `core/*`
- `libraries/led/ws2812`
- `libraries/led/rgb_utils`
- `libraries/led/led_effects`
- `libraries/led/led_segments`
- `libraries/led/ws2812_matrix`

## Примітки для PIC18

- Для RAM обмежуйте кількість одночасних анімацій.
- Для ефективності краще `auto_show = 0` і один `ws2812_show()` на цикл.
- У рушії немає blocking delays.
