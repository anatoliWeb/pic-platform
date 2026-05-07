[🇺🇦 Ukrainian version](./bitmap.ua.md)

# Bitmap Renderer Library

## Description

`bitmap` is a reusable display-agnostic bitmap rendering module for the pic-platform graphics system.
It renders bitmaps through `graphics_t` and never accesses hardware directly.

## Public API

- `bitmap_draw()`
- `bitmap_draw_transparent()`
- `bitmap_draw_region()`
- `bitmap_draw_scaled()` (optional)
- `bitmap_draw_inverted()` (optional)

## Bitmap Model

`bitmap_t` descriptor contains:

- `width`
- `height`
- `format`
- `data` pointer

Current supported format:

- `BITMAP_FORMAT_1BPP_ROW` (monochrome, row-based bit packing)

Prepared for future extension:

- grayscale formats
- RGB formats
- compressed formats

## 1bpp Row Format Notes

For `BITMAP_FORMAT_1BPP_ROW`:

- `bytes_per_row = (width + 7) / 8`
- bit order in byte: MSB first (`0x80 >> bit_index`)
- row-major storage

## Rendering and Clipping

- All pixel writes go through `graphics_draw_pixel()`.
- Clipping is handled by graphics core bounds checks.
- Region draw (`bitmap_draw_region`) supports sprite-sheet style rendering.

## Transparency

`bitmap_draw_transparent()` supports transparent bit skipping:

- `transparent_bit = 0` or `1`
- matching bits are ignored

## Performance Notes

- Integer-only math
- No dynamic memory
- Lightweight loops for embedded targets
- Optional scaling uses nearest-neighbor pixel replication

## Example

`example.c` demonstrates:

- icon rendering
- transparent overlay
- clipped region rendering
- scaled and inverted rendering
- simple bitmap animation

## Dependencies

- `core/*`
- `libraries/display/graphics`
