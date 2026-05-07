[English version](./screen_transitions.md)

# Бібліотека переходів між екранами

## Опис

`screen_transitions` забезпечує легкий неблокуючий рушій переходів для
зміни екранів в embedded UI-системах.

Вона не залежить від конкретного дисплея та апаратної платформи.

## Публічний API

- `screen_transition_init()`
- `screen_transition_start()`
- `screen_transition_update()`
- `screen_transition_draw()`
- `screen_transition_is_active()`
- `screen_transition_cancel()` (опціонально)
- `screen_transition_set_speed()` (опціонально)

## Примітки щодо Transition / Render Flow

Поля моделі переходу (`transition_t`):

- `type`
- `duration_ms`
- `progress` (0..1000)
- `active`
- `source_screen`
- `target_screen`
- `user_data`

Підтримувані типи переходів:

- fade
- slide_left
- slide_right
- slide_up
- slide_down

Процес оновлення:

- обчислює час, що минув, за допомогою `now_ms`
- оновлює цілочисельний progress
- автоматично завершується після завершення duration
- після завершення перемикає поточний екран на target

Процес малювання:

- якщо перехід неактивний: делегує малювання overlay/screen
- якщо перехід активний: малює source або target залежно від фази та додає легкий маркер
- уникає framebuffer blending на весь екран

## Примітки щодо анімації / продуктивності

- повністю неблокуюча робота
- без викликів delay
- лише цілочисельна математика
- без динамічної пам’яті
- PIC-friendly підхід із низьким використанням RAM

Стратегія для малої пам’яті:

- не потрібен повноекранний offscreen buffer
- перехід візуалізується легкими маркерами/progress bars
- детермінований час виконання

## Залежності

- `core/*`
- `libraries/ui/screen_manager`
- `libraries/ui/overlay_system`
- `libraries/display/graphics`
- `libraries/ui/sprite_buffer`

## Приклад

`example.c` демонструє:

- fade transition
- slide transition
- цикл анімації перемикання екранів
