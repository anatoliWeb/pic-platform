# Display Framework

## Overview

`display_framework` provides a small, portable display abstraction for embedded projects.
It unifies high-level drawing calls and delegates rendering to backend adapters.

This is not a GUI system. It is a thin portability layer.

## Public API

- `display_init()`
- `display_clear()`
- `display_update()`
- `display_draw_pixel()`
- `display_draw_char()`
- `display_draw_string()`
- `display_draw_number()`
- `display_draw_line()` (optional)
- `display_draw_rect()` (optional)

## Backend Abstraction Model

The framework uses a function-pointer backend:

- `clear(ctx)`
- `update(ctx)`
- `draw_pixel(ctx, x, y, color)`
- `draw_char(ctx, x, y, char, color)`

`display_t` stores backend callbacks, backend context pointer, and display size.

## Supported Backends (Initial)

- LCD HD44780 adapter
- Seven Segment adapter (single-digit style mapping)
- WS2812 Matrix adapter

Unsupported operations are no-op for limited-capability displays.

## Coordinate Model

Unified coordinates are `x, y` for all backends.
Backends may reinterpret or ignore unsupported dimensions (for example seven-segment).

## Text Rendering

- `display_draw_string()` and `display_draw_number()` are backend-independent entry points.
- Matrix backend includes a lightweight built-in 5x7 glyph subset (digits and basic symbols).
- LCD backend delegates character output directly to LCD driver.

## Memory Model

- No dynamic memory allocation.
- Framework does not own framebuffer memory.
- Backend context points to existing display objects (LCD, seven-segment, matrix).

## Portability Goals

- Same API for multiple display technologies.
- Easy backend switch without application rewrite.
- Works in main loop, scheduler tasks, or RTOS tasks.

## MCU Limitations Notes

- Keep draw calls lightweight on PIC18.
- Use backend-specific update rate tuning.
- Avoid large software fonts and heavy full-screen redraw loops.
