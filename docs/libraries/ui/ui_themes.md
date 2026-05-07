[🇺🇦 Ukrainian version](./ui_themes.ua.md)

# UI Themes Library

## Description

`ui_themes` is a reusable lightweight theme system for embedded UI rendering.
It defines semantic color roles and icon style hints for monochrome and RGB565 displays.

## Public API

- `ui_theme_get_default()`
- `ui_theme_get_dark()`
- `ui_theme_get_light()`
- `ui_theme_apply()`
- `ui_theme_get_color()`
- `ui_theme_set_color()` (optional)
- `ui_theme_get_icon_style()` (optional)

## Theme Model

`ui_theme_t` includes:

- `background`
- `foreground`
- `accent`
- `warning`
- `error`
- `success`
- `disabled`
- `border`
- `icon_style`
- `monochrome` mode flag

All roles are stored as 16-bit values (RGB565-compatible).

## Monochrome vs RGB565 Behavior

- RGB displays: raw 16-bit theme color is returned.
- Monochrome displays: `ui_theme_get_color()` maps each role to `0/1` (off/on).

This keeps one theme API for OLED and TFT backends.

## Display Compatibility

Designed to work with:

- OLED monochrome rendering through `graphics_t`
- ST7735 / ILI9341 RGB565 backends
- display framework integrations
- icon pack rendering flows

## Usage Notes

- Static const built-in themes minimize RAM usage.
- `ui_theme_apply()` copies selected preset into runtime theme.
- Runtime overrides can be applied with `ui_theme_set_color()`.

## Example

`example.c` demonstrates:

- dark/light switching
- themed panel rendering
- themed icon drawing
- monochrome mapping behavior

## Dependencies

- `core/*`
- `libraries/display/graphics`
- `libraries/ui/icon_pack`

## Embedded Constraints

- no dynamic memory
- integer-only logic
- small ROM/RAM footprint
