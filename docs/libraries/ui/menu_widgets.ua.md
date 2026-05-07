[🇬🇧 English version](./menu_widgets.md)

# Бібліотека Menu Widgets

## Опис

`menu_widgets` — легкий універсальний шар віджетів для embedded UI.
Бібліотека інтегрується з `menu_core`, `menu_renderer`, `graphics`, `ui_themes` і залишається display/input agnostic.

## Публічний API

- `menu_widget_init()`
- `menu_widget_draw()`
- `menu_widget_update()`
- `menu_widget_bind_menu_item()`

## Нотатки по Моделі Віджетів

`menu_widget_t` містить:

- тип віджета
- позицію та розміри
- локальні кольорові поля
- pointer на значення + min/max/step
- pointer на список опцій + кількість
- опційний bind до menu item
- callbacks для draw/update

Підтримувані типи:

- checkbox
- toggle
- progress bar
- numeric value
- option selector
- slider (опційний alias над progress)
- text field (placeholder)
- graph (placeholder)

## Нотатки по Рендерингу / Інтеграції

- Рендеринг використовує лише примітиви `graphics_t`.
- Кольори теми читаються через `ui_theme_t`.
- Прямий доступ до OLED/TFT hardware відсутній.
- Віджети можуть рендеритись разом із виводом `menu_renderer`.

Інтеграція з меню:

- Віджети можуть прив'язуватись до `menu_item_t` для подальшого interactive editing.
- Поточна версія фокусується на детермінованій візуалізації value/toggle станів.

## Пам'ять і Продуктивність

- без dynamic memory
- integer-only логіка
- без великих тимчасових буферів
- lightweight шляхи рендерингу

## Приклад

`example.c` демонструє:

- checkbox
- toggle
- progress і slider
- numeric
- option selector
- themed rendering поверх menu background

## Залежності

- `core/*`
- `libraries/ui/menu_core`
- `libraries/ui/menu_renderer`
- `libraries/display/graphics`
- `libraries/ui/ui_themes`
