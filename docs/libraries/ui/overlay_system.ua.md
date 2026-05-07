[English version](./overlay_system.md)

# Бібліотека системи overlay

## Опис

`overlay_system` забезпечує легкий багатошаровий рендеринг для embedded UI.
Вона не залежить від конкретного дисплея та системи введення.

Модуль керує кількома overlay-елементами з урахуванням z-order, видимості, callback-функцій оновлення
та callback-функцій малювання.

## Публічний API

- `overlay_system_init()`
- `overlay_add()`
- `overlay_remove()`
- `overlay_update()`
- `overlay_draw()`
- `overlay_set_visible()` (опціонально)
- `overlay_bring_to_front()` (опціонально)

## Примітки щодо Overlay / Layer Flow

Поля моделі overlay (`overlay_t`):

- `id`
- `visible`
- `z_index`
- `x`, `y`, `width`, `height`
- `user_data`
- `on_update()`
- `on_draw()`
- `on_event()`

Порядок малювання шарів:

1. активний екран (`screen_manager_draw()`)
2. видимі overlays (відсортовані за `z_index`)
3. popup manager (`popup_draw()`)

## Примітки щодо Z-Order / Events

- Overlays зберігаються у пулі фіксованого розміру (`OVERLAY_SYSTEM_MAX_OVERLAYS`).
- Z-order підтримується через insertion sort після операцій add/front.
- `overlay_bring_to_front()` піднімає вибраний overlay через збільшення z-index.

Інтеграція подій:

- Зчитує останню UI-подію з `ui_events`.
- Передає події видимим overlays через callback `on_event()`.
- Popup manager залишається незалежним і оновлюється окремо.

## Модель рендерингу

- Рендеринг використовує `graphics_t` та callback-функції.
- Можливе опціональне використання тем, іконок і спрайтів у callback-функціях малювання overlay.
- Немає прямого доступу до OLED/TFT hardware.

## Детермінована модель пам’яті

- лише статичне/фіксоване виділення пам’яті
- без динамічної пам’яті
- детермінована складність update/draw

## Залежності

- `core/*`
- `libraries/ui/screen_manager`
- `libraries/ui/popup_manager`
- `libraries/ui/ui_events`
- `libraries/display/graphics`
- `libraries/ui/ui_themes`
- `libraries/ui/icon_pack`
- `libraries/ui/sprite_buffer`

## Приклад

`example.c` демонструє:

- overlay статус-бара
- debug overlay
- popup, що рендериться поверх overlays
- періодичний цикл update/draw
