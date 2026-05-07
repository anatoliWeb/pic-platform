[🇺🇦 Ukrainian version](./graphics.ua.md)

# Graphics Core Library

## Description

`graphics` is a reusable display-agnostic rendering core for monochrome and custom embedded display backends.
It is designed to be shared by SSD1306, SH1106, and future display targets.

## Public API

- `graphics_init()`
- `graphics_draw_pixel()`
- `graphics_draw_line()`
- `graphics_draw_rect()`
- `graphics_fill_rect()`
- `graphics_draw_circle()`
- `graphics_draw_char()`
- `graphics_draw_string()`
- `graphics_draw_triangle()` (optional)
- `graphics_fill_circle()` (optional)
- `graphics_flush()`

## Graphics Context Model

`graphics_t` includes:

- `width`, `height`
- framebuffer pointer + size
- optional pixel callbacks (`set/get`)
- optional flush callback
- user context pointer

This allows both direct framebuffer rendering and custom backend rendering.

## Rendering Algorithms

- Line: Bresenham algorithm
- Circle: midpoint circle algorithm
- Rectangle/fill: line-based integer rasterization
- Font: built-in 5x7 ASCII

Integer math only, no floating-point operations.

## Clipping and Safety

- Every pixel draw is bounds-checked.
- Out-of-range rendering is clipped by safe pixel rejection.
- Prevents framebuffer out-of-bounds writes.

## Display Integration

- No hardware bus access in graphics core.
- Works with SSD1306 framebuffer by passing `framebuffer` and `flush_cb`.
- Prepared for SH1106 and other future display backends.

## Example

`example.c` demonstrates:

- text rendering
- primitive rendering
- simple animation
- SSD1306 integration via framebuffer + flush callback

## Dependencies

- `core/*`
- optional integration with `libraries/display/display_framework`

## Embedded Constraints

- No dynamic memory
- Integer-only math
- Lightweight state and algorithms
