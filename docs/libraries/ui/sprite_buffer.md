[🇺🇦 Ukrainian version](./sprite_buffer.ua.md)

# Sprite Buffer Library

## Description

`sprite_buffer` is a reusable offscreen sprite/framebuffer utility layer for embedded UI rendering.
It is display-agnostic and integrates with `graphics` and `bitmap` modules.

## Public API

- `sprite_init()`
- `sprite_clear()`
- `sprite_draw_pixel()`
- `sprite_fill_rect()` (optional)
- `sprite_draw_bitmap()`
- `sprite_draw_sprite()`
- `sprite_copy_region()` (optional)
- `sprite_blit()`
- `sprite_blit_transparent()` (optional)

## Sprite Framebuffer Notes

`sprite_t` contains:

- `width`, `height`
- `format`
- `framebuffer`
- `framebuffer_size`

Current format:

- `SPRITE_FORMAT_1BPP_ROW`

Storage model:

- row-based bit packing
- `bytes_per_row = (width + 7) / 8`
- MSB-first bit order per byte

## Rendering and Blit Notes

- All drawing happens in local sprite framebuffer (offscreen).
- `sprite_blit()` copies non-zero sprite pixels to `graphics_t` target.
- `sprite_blit_transparent()` skips selected transparent bit value.
- Clipping safety is ensured by target `graphics_draw_pixel()` bounds handling.

## Offscreen Rendering Model

Typical workflow:

1. Render UI element into sprite buffer
2. Compose/copy sprite regions
3. Blit sprite to final graphics surface
4. Flush display in display backend

This is suitable for animation buffering and UI panel composition.

## Dependencies

- `core/*`
- `libraries/display/graphics`
- `libraries/display/bitmap`

## Embedded Constraints

- No dynamic memory
- Integer-only logic
- Lightweight loops and RAM-friendly layout
