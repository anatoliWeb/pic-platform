[🇺🇦 Ukrainian version](./menu_renderer.ua.md)

# Menu Renderer Library

## Description

`menu_renderer` is a reusable display-agnostic visualization layer for `menu_core` state.
It renders menu items using `graphics_t`, optional icons, and UI themes.

It does not read hardware input and does not access display hardware directly.

## Public API

- `menu_renderer_init()`
- `menu_renderer_draw()`
- `menu_renderer_set_theme()` (optional)
- `menu_renderer_set_icons()` (optional)
- `menu_renderer_set_layout()` (optional)

## Rendering / Layout Notes

Supported behavior:

- vertical menu list layout
- selected row highlight
- submenu indicator arrow
- toggle state indicator
- value item visualization

Layout controls:

- line height
- visible viewport rows
- X/Y paddings

## Scrolling / Theme Notes

Scrolling model:

- renderer tracks `top_index`
- selected item index is compared with viewport window
- auto-scroll updates `top_index` when selected item moves outside visible region

Theme integration:

- role-based colors from `ui_theme_t`
- monochrome and RGB theme values are accepted
- current implementation uses non-zero/zero drawing semantics in generic graphics path

Icon integration:

- optional item-type icon rendering via `icon_pack`
- submenu arrow and state icons are drawn at right edge

## Display Compatibility

Renderer is display-agnostic and works with any backend that provides a valid `graphics_t` context:

- OLED (SSD1306/SH1106 via graphics framebuffer)
- TFT (ST7735/ILI9341 via graphics callback context)
- display framework-based compositions

## Example

`example.c` demonstrates:

- themed menu rendering
- icon-based item rendering
- auto-scrolling behavior
- theme switching
- OLED path and notes for TFT reuse

## Dependencies

- `core/*`
- `libraries/ui/menu_core`
- `libraries/display/graphics`
- `libraries/display/display_framework`
- `libraries/ui/icon_pack`
- `libraries/ui/ui_themes`

## Embedded Constraints

- no dynamic memory
- integer-only logic
- lightweight viewport-based rendering
