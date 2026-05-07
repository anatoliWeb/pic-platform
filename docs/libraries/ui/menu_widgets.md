[🇺🇦 Ukrainian version](./menu_widgets.ua.md)

# Menu Widgets Library

## Description

`menu_widgets` is a reusable lightweight widget layer for embedded UI systems.
It integrates with `menu_core`, `menu_renderer`, `graphics`, and `ui_themes` while staying display/input agnostic.

## Public API

- `menu_widget_init()`
- `menu_widget_draw()`
- `menu_widget_update()`
- `menu_widget_bind_menu_item()`

## Widget Model Notes

`menu_widget_t` includes:

- widget type
- position and size
- local color fields
- value binding pointer and min/max/step
- option list pointer/count
- optional menu item binding
- draw callback and update callback hooks

Supported widget types:

- checkbox
- toggle
- progress bar
- numeric value
- option selector
- slider (optional alias over progress)
- text field (placeholder)
- graph (placeholder)

## Rendering / Integration Notes

- Rendering uses only `graphics_t` primitives.
- Theme colors are read through `ui_theme_t` role helpers.
- No direct OLED/TFT hardware access.
- Widget layer can be drawn alongside `menu_renderer` output.

Menu integration:

- Widgets can bind to `menu_item_t` for future interactive editing logic.
- Current implementation focuses on deterministic visualization with value/toggle states.

## Memory and Performance

- no dynamic memory
- integer-only logic
- no large temporary buffers
- lightweight shape/text rendering paths

## Example

`example.c` demonstrates:

- checkbox widget
- toggle widget
- progress and slider widgets
- numeric widget
- option selector widget
- themed rendering with menu background

## Dependencies

- `core/*`
- `libraries/ui/menu_core`
- `libraries/ui/menu_renderer`
- `libraries/display/graphics`
- `libraries/ui/ui_themes`
