[Ukrainian version](./overlay_system.ua.md)

# Overlay System Library

## Description

`overlay_system` provides lightweight layered rendering for embedded UI.
It is display-agnostic and input-agnostic.

The module manages multiple overlays with z-order, visibility, update callbacks,
and draw callbacks.

## Public API

- `overlay_system_init()`
- `overlay_add()`
- `overlay_remove()`
- `overlay_update()`
- `overlay_draw()`
- `overlay_set_visible()` (optional)
- `overlay_bring_to_front()` (optional)

## Overlay / Layer Flow Notes

Overlay model (`overlay_t`) fields:

- `id`
- `visible`
- `z_index`
- `x`, `y`, `width`, `height`
- `user_data`
- `on_update()`
- `on_draw()`
- `on_event()`

Layer draw order:

1. active screen (`screen_manager_draw()`)
2. visible overlays (sorted by `z_index`)
3. popup manager (`popup_draw()`)

## Z-Order / Event Notes

- Overlays are stored in a fixed-size pool (`OVERLAY_SYSTEM_MAX_OVERLAYS`).
- Z-order is maintained by insertion sort after add/front operations.
- `overlay_bring_to_front()` raises selected overlay z-index.

Event integration:

- Reads latest UI event from `ui_events`.
- Routes events to visible overlays via `on_event()` callback.
- Popup manager remains independent and is updated separately.

## Rendering Model

- Rendering uses `graphics_t` and callbacks.
- Optional usage with themes/icons/sprites in overlay draw callbacks.
- No direct OLED/TFT hardware access.

## Deterministic Memory Model

- static/fixed allocation only
- no dynamic memory
- deterministic update/draw complexity

## Dependencies

- `core/*`
- `libraries/ui/screen_manager`
- `libraries/ui/popup_manager`
- `libraries/ui/ui_events`
- `libraries/display/graphics`
- `libraries/ui/ui_themes`
- `libraries/ui/icon_pack`
- `libraries/ui/sprite_buffer`

## Example

`example.c` demonstrates:

- status bar overlay
- debug overlay
- popup rendered above overlays
- periodic update/draw loop
