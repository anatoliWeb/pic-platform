# WS2812 Matrix Library

## Description

`ws2812_matrix` is a lightweight 2D mapping layer for WS2812/WS2812B LEDs.  
It converts matrix coordinates `(x, y)` into a linear LED index and reuses the existing WS2812 driver for output.

## Public API

- `ws2812_matrix_init()`
- `ws2812_matrix_get_index()`
- `ws2812_matrix_set_pixel()`
- `ws2812_matrix_clear()`
- `ws2812_matrix_fill()`
- `ws2812_matrix_show()`
- `ws2812_matrix_draw_line()` (optional helper)
- `ws2812_matrix_draw_rect()` (optional helper)

## Supported Matrix Layouts

- Row-major
- Column-major
- Serpentine (zigzag) or straight
- Origins:
  - top-left
  - top-right
  - bottom-left
  - bottom-right
- Optional full-chain reverse mapping

## Mapping Model

1. Validate `(x, y)` bounds.
2. Apply origin transform (mirror X/Y depending on selected corner).
3. Compute base index using row-major or column-major mode.
4. Apply serpentine swap on odd rows/columns.
5. Apply reverse mapping if enabled.

## Example Usage

```c
ws2812_matrix_config_t cfg;
cfg.strip = &strip;
cfg.width = 8;
cfg.height = 8;
cfg.layout = WS2812_MATRIX_LAYOUT_ROW_MAJOR;
cfg.serpentine = 1;
cfg.origin = WS2812_MATRIX_ORIGIN_TOP_LEFT;
cfg.reverse = 0;

ws2812_matrix_init(&matrix, &cfg);
ws2812_matrix_set_pixel(&matrix, 3, 2, rgb_color(255, 0, 0));
ws2812_matrix_show(&matrix);
```

## Dependencies

- `core/*`
- `libraries/led/ws2812`
- `libraries/led/rgb_utils`

## RAM and Performance Notes

- No dynamic memory allocation.
- Uses existing WS2812 framebuffer.
- Mapping is integer-only, no floating point.
- `show()` timing is still handled by WS2812 low-level implementation.

## Limitations

- PIC18 RAM limits matrix size through pixel buffer size.
- Library does not include fonts/sprites; it only provides 2D mapping and drawing helpers.
