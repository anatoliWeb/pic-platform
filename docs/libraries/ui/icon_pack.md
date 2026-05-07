[🇺🇦 Ukrainian version](./icon_pack.ua.md)

# Icon Pack Library

## Description

`icon_pack` is a reusable embedded icon asset library for the pic-platform UI layer.
It is display-agnostic and works with `graphics` and `bitmap` modules.

## Public API

- `icon_get()`
- `icon_draw()`
- `icon_draw_scaled()` (optional)
- `icon_draw_inverted()` (optional)

## Icon Format / Storage Notes

`ui_icon_t` contains:

- `width`
- `height`
- `format`
- `data` pointer

Current format:

- monochrome `BITMAP_FORMAT_1BPP_ROW`

Storage model:

- icon bitmaps are `static const` data
- optimized for flash/ROM usage
- no runtime allocations

## Implemented Icon Groups

System:

- settings
- warning
- error
- info

Connectivity:

- wifi
- bluetooth
- ethernet

Status:

- battery
- charging
- temperature
- clock

Controls:

- play
- stop
- pause
- menu
- back
- arrows (up/down/left/right)

## Rendering Integration Notes

- `icon_draw()` internally uses bitmap renderer (`bitmap_draw_transparent`).
- Rendering target is `graphics_t` context.
- Clipping is handled by graphics pixel bounds checks.
- No hardware-specific code in icon pack.

## Example

`example.c` demonstrates:

- generic icon grid rendering
- OLED icon demo (SSD1306 + graphics context)
- TFT icon demo (ST7735 bitmap rendering path)
- simple icon animation

## Dependencies

- `core/*`
- `libraries/display/bitmap`
- `libraries/display/graphics`

## Embedded Constraints

- no dynamic memory
- integer-only logic
- ROM-friendly static assets
